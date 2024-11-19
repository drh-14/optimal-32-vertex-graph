// Read the comment on the bottom function first to better understand what variables mean
#include "gendef.h"

static int n, k, t, mid_max, splitlevel;
static unsigned long to_store, to_print,count;
static FILE *resultFile, *lstfile, *autfile;
static unsigned int jobs, jobNumber;
static int store_all, print_all;

static int jump , i_1, j_1, f_0, f_1, girth_exact;
static int **g, **l, **part, **einsen, **transp, **zbk;
static int *kmn, *grad, *lgrad, *lastcode;
static unsigned long calls, dez, anz;
static long fpos;

/*
 with Maxtest at mid_max
 WARNING: "vm" has been modified: vm = n + 1 if grad[n] >= 1

 last change:
 instead of if(vm <= n && ...) vm++ now if(vm <= my) vm = my + 1

 Splitting the generation into multiple steps
 is now possible

 Shortcode implemented
*/

#ifdef STAB  
static int **aut;
static int  erz, merz;

// The function prints each 2D array of automorphism and its order.
static void stabprint(){
	int i, j;
	long order = 1;
	int next,last=0,mult=1;

	for(i=1;i<=erz;i++)
		{
		next= aut[i][0];
		if(last == next){
		mult++;
        }
		else{
		last = next;
		order *= mult;
		mult = 2;
		}
		fprintf(autfile,"\n%d :",next);
		for(j=1;j<=n;j++)
		fprintf(autfile," %d", aut[i][j]);
		}
	fprintf(autfile,"\nOrder: %ld\n\n", order * mult);
}
#endif

// Outputs an error message and aborts the program if memory is full.
static void err(){
	fprintf(stderr, "Abort error: Memory full");
	exit(0);
}

// Prints the list of neighboring nodes for each node in the graph
static void neighbor_list(){
	int i, j;
	for(i = 1;i <= n;i++)
		{
		fprintf(autfile,"\n%d : ", i);
		for(j = 1; j <= grad[i]; j++)
		fprintf(autfile,"%d ",l[i][j]);
		}
	fprintf(autfile,"\n");
}

/*
 (f_0)f_1 is the smallest node for which, based on the girth and the degree,
  an edge is (not) predetermined.
 Example: k=3, t=3 -> f1=2, f0=5
          k=3, t=5 -> f1=5, f0=11
*/

#ifndef SHORTCODE

static void compressToFile(){
	SCHAR i,j,c;
	SCHAR *store;
	store=g[0];

	for(i=f1;i<f0;i++)
		store[i]=2;
	for(i=f0;i<=n;i++)
		store[i]=1;

	for(i=f1;i<=n;i++)
		for(j=store[i];j<=k;j++)
		{
		c=l[i][j];
		store[c]++;
		putc(c,lstfile);
		}
}

#else
static void compressToFile()
{
 int i, j, c, equ = 1;
 unsigned int h = 0;

 for(i=1;i<=n;i++)
     for(j=1;j<=k;j++)
	{
	 c=l[i][j];
	 if(c>i)
	   {
	    if(equ&&c!=lastcode[++h])
	      {
	       equ=0;
	       putc(h - 1,lstfile);  /* Number of matching edges */
	       lastcode[h] = c;
	       putc(c,lstfile);   /* Only for n < 256 */
	      }
	    else if(!equ){
	       lastcode[++h]=c;
	       putc(c,lstfile);
	      }
	   }
	}
}
#endif

/*
 girthStart determines the girth and returns it or 0 if node 3 does not lie on the first girth circle.
 It is only called when the first cycle has just been closed.
*/
static int girthStart(){  // girth refers to the length of the shortest cycle in a graph
 int tw = 2,last = 1,now = 2,next;
 while(now != 3){
       next = l[now][1];
       if(next == last)
	  next = l[now][2];
       if(next == 0 || next == 2){
	  return(0);
      }
       last = now;now = next;tw++;
      }
 return(tw);
}

/*
 girthCheck2 tests whether the girth is reduced after inserting the edge (mx , my).
 If yes, the new girth is returned (i.e., G is not max). Otherwise, 127 is returned.
*/

static int girthCheck2(mx, my, tw)
int mx, my, tw;
{
 int *status, *xnachb, *ynachb;
 int welle = 1,a,i,j,x,y;
 xnachb = l[mx]; ynachb=l[my];

 if(tw == 4){
    for(i = 1;i < grad[mx];i++)
       {
	x= xnachb[i];
	for(j = 1;j < grad[my];j++)
	    if(x == ynachb[j])
	       return(3);
       }
    return(127);
   }

 if(tw == 5)
   {
    for(i = 1;i < grad[mx];i++){
	x = xnachb[i];
	for(j = 1;j < grad[my];j++)
	   {
	    y= ynachb[j];
	    if(x == y)
	       return(3);
	    if(g[x][y])
	       return(4);
	   }
       }
    return(127);
   }

 status=zbk[0];
 for(i = 1;i <= n;i++) {status[i]=0;}
 status[mx] = 1;status[my] = 2;
 zbk[2][1] = my;zbk[2][0] = 1;
 for(i = 1;i < grad[mx];i++){
     x = xnachb[i];
     zbk[3][i] = x;
     status[x] = 3;
    }
 zbk[3][0] = grad[mx]-1;
 while(++welle < tw - 1){
       a=0;
       for(i=1;i<= zbk[welle][0];i++){
	   x=zbk[welle][i];
	   for(j=1;j<=grad[x];j++)
	      {
	       y=l[x][j];
	       if(status[y]==0)
		 {
		  status[y]=welle+2;
		  zbk[welle+2][++a]=y;
		 }
	       else
	       if(status[y]==welle+1)
		  return(welle+1);
	      }
	  }
       zbk[welle+2][0]=a;
      }
 return(127);
}

/*
 ongirth0 checks for odd girth `tw` 
 whether node `v` lies on a girth circle. 
 If so, 1 is returned; otherwise, 0.
*/
static int ongirth_0(v,tw)
int v,tw;
{
 int *status,last,a,h,i,j=0,x,y;
 status=zbk[0];
 for(i = 1;i <= n;i++)status[i]=0;
 status[v]=(-1);
 for(h=1;h<=k;h++)
    {
     last=tw/2+h-2;
     x=l[v][h];status[x]=h;
     zbk[last][1]=x;zbk[last][0]=1;
     while(last>h)
	  {
	   a=0;
	   for(i=1;i<=zbk[last][0];i++)
	      {
	       x=zbk[last][i];
	       for(j=1;j<=k;j++)
		  {
		   y=l[x][j];
		   if(status[y]==0)
		     {
		      status[y]=h;
		      zbk[last-1][++a]=y;
		     }
		  }
	      }
	   zbk[--last][0]=a;
	  }
    }
 for(h=1;h<k;h++)
    {
     for(i=1;i<=zbk[h][0];i++)
	{
	 x=zbk[h][i];
	 for(j=1;j<=k;j++)
	    {
	     y=l[x][j];
	     if(status[y]>=0)
		status[y]=h;
	    }
	}
     for(last=h+1;last<=k;last++)
	{
	 for(i=1;i<=zbk[last][0];i++)
	    {
	     x=zbk[last][i];j=0;
	     for(j=1;j<=k;j++)
		{
		 y=l[x][j];
		 if(status[y]==h)
		    return(1);
		}
	    }
	}
    }
 return(0);
}


// Also checks if node v is part of a cycle of girth tw
static int ongirth1(v,tw)
int v,tw;
{
 int *status,last,a,h,i,j=0,x,y;
 status=zbk[0];
 for(i=1;i<=n;i++)status[i]=0;
 status[v]=(-1);
 for(h=1;h<=k;h++)
    {
     last=tw/2+h-1;
     x=l[v][h];status[x]=h;
     zbk[last][1]=x;zbk[last][0]=1;
     while(last>h)
	  {
	   a=0;
	   for(i=1;i<=zbk[last][0];i++)
	      {
	       x=zbk[last][i];
	       for(j=1;j<=k;j++)
		  {
		   y=l[x][j];
		   if(status[y]==0)
		     {
		      status[y]=h;
		      zbk[last-1][++a]=y;
		     }
		  }
	      }
	   zbk[--last][0]=a;
	  }
     if(h>1)
       {
	for(i=1;i<=zbk[h][0];i++)
	   {
	    x=zbk[h][i];
	    for(j=1;j<=k;j++)
	       {
		y=l[x][j];
		if(status[y]>0)
		   return(1);
	       }
	   }
       }
    }
 return(0);
}

// Determines if a specific node v participates in a cycle of girth tw.
static int isNodeInCycle(v,tw)
int v,tw;
{
 int i,j,x;
 if(tw==3)
   {
    for(i=1;i<=k;i++)
       {
	x=l[v][i];
	for(j=i+1;j<=k;j++)
	    if(g[x][l[v][j]])
	       return(1);
       }
    return(0);
   }
 else
 if(tw%2==0)
    return(ongirth_0(v,tw));
 else
    return(ongirth1(v,tw));
}

// Finds and updates the maximum edge pair based on conditions specified in the loops.
static void findMaxEdgePair(i,j){
int i,j;
 int r,s,x,y,z,e;
 i_1 = 1; j_1 = j;
 for(r = 1;r <= i;r++){
     e = lgrad[r];
     for(s = r + 1;e < k && (r < i || s < j);s++)
	{
	 z = g[r][s];e += z;
	 if(z)
{
	    x=kmn[r];y=kmn[s];
	    if(x>y)
	      {z=x;x=y;y=z;}
	    if(x>i_1||(x==i_1&&y>j_1))
	      {i_1=x;j_1=y;}
	   }
	}
    }
 x=kmn[i];y=kmn[j];
 if(x>y)
   {z=x;x=y;y=z;}
 if(x > i_1 ||(x == i_1 && y>j_1))
   {i_1 = x; j_1 = y;}
}

// Reverses the permutation of nodes, modifying the kmn array.
static void transpinv(mperm)
int *mperm;
{
 int i,x,re,li;
 i=2*mperm[0];
 while(i>0)
      {
       re=mperm[i--];
       li=mperm[i--];
       x=kmn[re];
       kmn[re]=kmn[li];
       kmn[li]=x;
      }
}

// Finds maximum values in a block of a given row, updating permutations.
static int maxInBlock(zeile,mperm,e,li,re)
int *zeile,*mperm;
int e,li,re;
{
 int i,x;
 i= 2* mperm[0];
 while(e>=0&&li<=re)
      {
       while(zeile[kmn[li]])
	    {
	     e--;
	     if(++li>re)
		return(e);
	    }
       if(li==re)
	  return(e);
       while(zeile[kmn[re]]==0)
	     if(--re==li)
		return(e);
       mperm[++i]=li;
       mperm[++i]=re;
       mperm[0]++;
       x=kmn[li];
       kmn[li]=kmn[re];
       kmn[re]=x;
       li++;re--;e--;
      }
 return(e);
}

// Handles maximum values in a specific row, details are cut off in the provided code snippet.
static int maxInRow(tz)
int tz;
{
 int b,*zeile,*block,*eintr,*mperm;
 int e,li,re,erg=0;
 zeile=g[kmn[tz]];
 block=part[tz];
 eintr=einsen[tz];
 mperm=transp[tz];
 mperm[0]=0;

 for(b=1;b<=block[0]&&erg==0;b++)
    {
     e=eintr[b];
     li=block[b];re=block[b+1]-1;
     erg=maxInBlock(zeile,mperm,e,li,re);
    }
 if(erg==0)
    return(0);
 else
 if(erg>0)
    return(1);   
 findMaxEdgePair(tz,li+e);
 return(-1);
}

/*
maxrekneu recursively explores permutations of node configurations, looking for an optimal arrangement based on the criteria defined in maxinzeile. This is a recursive backtracking function, leveraging swaps and recursive calls to systematically explore and record configurations that meet its conditions.
*/
static int maxrekneu(tz)
int tz;
{
 int i,x,e;
 int erg;
#ifdef STAB
 int *aut1;
 if(tz>=n-1)
   {
    aut1=aut[++erz];
    for(i=1;i<=n;i++)
       aut1[i]=kmn[i];
    return(0);
   }
#else
 if(tz>=n-1)
    return(0);
#endif
 /*x=kmn[tz];*/
 erg=maxInRow(tz);
 if(erg==0)
    erg=maxrekneu(tz+1);
 transpinv(transp[tz]);
 e=part[0][tz];
 for(i=tz+1;i<=e&&erg==1;i++)
    {
     x=kmn[tz];
     kmn[tz]=kmn[i];
     kmn[i]=x;
     erg= maxInRow(tz);
     if(erg==0)
	erg=maxrekneu(tz+1);
     transpinv(transp[tz]);
     x=kmn[tz];
     kmn[tz]=kmn[i];
     kmn[i]=x;
    }
 return(erg);
}

static int start_max_search(int tw) {
    int i, j, e;
    int erg;

#ifdef STAB
    erz = 0; // Reset 'erz' counter if STAB is defined
#endif

    // Loop through stabilizing transformations for points
    for (i = n - 1; i > 1; i--) {
        // Stabilizers: Stab(n-2), ..., Stab(0) = S(n)
        e = part[0][i];
        for (j = i + 1; j <= e; j++) {
            kmn[i] = j;
            kmn[j] = i; // Swap elements (transposition (i, j))
            
            erg = maxInRow(i); // Check if this row's configuration is optimal
            
            if (erg == 0) {
                erg = maxrekneu(i + 1); // Traverse subclass (i, j)Stab{0,...,i}
            }

#ifdef STAB
            if (erg == 0) {
                aut[erz][0] = i; // If erg == 0, store index i in aut array
            }
#endif

            transpinv(transp[i]); // Reverse the transformation on transp[i]
            kmn[i] = i; // Restore original state for kmn[i]
            kmn[j] = j; // Restore original state for kmn[j]

            if (erg == -1) {
                return 0; // Early exit if erg indicates a stopping condition
            }
        }
    }

    // Loop through subclasses of stabilizer Stab(1)
    for (j = 2; j <= n; j++) {
        if (isNodeInCycle(j, tw)) { // Check if node j is in the cycle
            kmn[1] = j;
            kmn[j] = 1; // Swap elements (transposition (1, j))

            erg = maxInRow(1); // Check if this row's configuration is optimal

            if (erg == 0) {
                erg = maxrekneu(2); // Traverse subclass (1, j)Stab{0,...,1}
            }

#ifdef STAB
            if (erg == 0) {
                aut[erz][0] = 1; // Store 1 in aut array if erg is 0
            }
#endif

            transpinv(transp[1]); // Reverse the transformation on transp[1]
            kmn[1] = 1; // Restore original state for kmn[1]
            kmn[j] = j; // Restore original state for kmn[j]

            if (erg == -1) {
                return 0; // Early exit if erg indicates a stopping condition
            }
        }
    }

    return 1; // Return 1 if all configurations were processed successfully
}


static int check_max_in_row(int tz) {
    int e, li, re, erg = 0;
    int b, *zeile, *block, *eintr, *mperm;

    // Initialize pointers to arrays
    zeile = g[kmn[tz]];   // Row data for the current configuration of tz
    block = part[tz];     // Block partition information for tz
    eintr = einsen[tz];   // Entry information for tz
    mperm = transp[tz];   // Permutation data for tz

    mperm[0] = 0;  // Reset the permutation index
    
    // Loop through each block in the partition
    for (b = 1; b <= block[0] && erg == 0; b++) {
        e = eintr[b];           // Get entry index in einsen for block b
        li = block[b];          // Start index of the current block
        re = block[b + 1] - 1;  // End index of the current block
        
        // Check for maximum configuration within the block
        erg = maxInBlock(zeile, mperm, e, li, re); 

        // maxinblock returns a value indicating the optimality of the block,
        // and we stop early if an optimal configuration is found.
    }

    // Determine final return value based on erg
    if (erg == 0)
        return 0;        // No valid configuration found in any block
    else if (erg > 0)
        return 1;        // Valid configuration found in at least one block
    
    return -1;           // Indicates an error or unhandled case
}


static int find_max_recursively(int tz) {
    int i, x, e;
    int erg;

    // Base case: if tz has reached the last index, return 0 as no further recursion is needed
    if (tz == n - 1)
        return 0;

    // Evaluate the current row configuration using maxinzeile1
    erg = maxinzeile1(tz);

    // If no valid configuration is found, continue recursively with the next index (tz + 1)
    if (erg == 0)
        erg = find_max_recursively(tz + 1);

    // Invert the transformation on the current index
    transpinv(transp[tz]);

    // Get the limit of possible elements for swapping, defined by `part[0][tz]`
    e = part[0][tz];

    // Loop over possible swaps for elements from tz+1 up to e
    for (i = tz + 1; i <= e && erg == 1; i++) {
        // Swap elements at tz and i
        x = kmn[tz];
        kmn[tz] = kmn[i];
        kmn[i] = x;

        // Re-evaluate the row after the swap
        erg = maxinzeile1(tz);

        // If no valid configuration found, continue recursion with next index (tz + 1)
        if (erg == 0)
            erg = find_max_recursively(tz + 1);

        // Undo the transformation after recursion to reset the state
        transpinv(transp[tz]);

        // Swap elements back to restore original configuration
        x = kmn[tz];
        kmn[tz] = kmn[i];
        kmn[i] = x;
    }

    // Return the result of the recursive search
    return erg;
}


static int start_max_search(int vm) {
    int i, j, e;
    int erg;

    // Loop through elements from `vm` down to 2
    for (i = vm; i > 1; i--) {
        // Set the upper limit of elements for swapping
        e = part[0][i];

        // Loop over potential swaps from `i+1` to `e`
        for (j = i + 1; j <= e; j++) {
            // Swap elements at i and j
            kmn[i] = j;
            kmn[j] = i;

            // Evaluate the row configuration at `i` using maxinzeile1
            erg = maxinzeile1(i);

            // If no valid configuration found, try recursively at the next index
            if (erg == 0 && i < n - 1)
                erg = maxrekneu1(i + 1);

            // Reverse the transformation at `i`
            transpinv(transp[i]);

            // Reset elements after recursion
            kmn[i] = i;
            kmn[j] = j;

            // If an invalid state is detected, return 0
            if (erg == -1)
                return 0;
        }
    }

    // Check configurations for elements from 2 to `vm`
    for (j = 2; j <= vm; j++)
        if (grad[j] == k) {  // Check if element `j` matches a given condition (e.g., has degree `k`)
            kmn[1] = j;
            kmn[j] = 1;

            // Evaluate the row configuration at index 1
            erg = maxinzeile1(1);

            // If no valid configuration found, check recursively from index 2
            if (erg == 0)
                erg = maxrekneu1(2);

            // Reverse transformation for `transp[1]`
            transpinv(transp[1]);

            // Reset elements after recursion
            kmn[1] = 1;
            kmn[j] = j;

            // If an invalid state is detected, return 0
            if (erg == -1)
                return 0;
        }

    // If no errors encountered, return 1 indicating successful search completion
    return 1;
}


/*
 semiverf creates part[x+1],
 the refinement of part[x]
 based on the entries in line x.
 semiverf is only called
 if line x is filled.
*/
static void initialize_next_partition(int x) {
    int *nextpart, *block, blanz, blockgr, einsanz, i;

    // Set pointers to the current block and the next partition
    block = part[x];
    blanz = block[0];                 // Number of blocks in `part[x]`
    nextpart = part[x + 1];           // Prepare next partition block
    einsanz = einsen[x][1];           // Number of special elements (e.g., "ones") in the first sub-block
    blockgr = block[2] - block[1];    // Size of the first sub-block in the partition

    // Set up next partition based on special cases for block sizes
    if (blockgr == 1) {
        nextpart[0] = 0;                 // Empty the next partition
        part[0][x + 1] = x + 1;          // Set next partition start index
    } 
    else if (einsanz == 1) {
        nextpart[1] = x + 2;             // Mark the next sub-block start
        nextpart[0] = 1;                 // Set size for next partition
        part[0][x + 1] = x + 1;          // Mark the end of the next block
    } 
    else if (einsanz == blockgr || einsanz == 0) {
        nextpart[1] = x + 2;
        nextpart[0] = 1;
        part[0][x + 1] = x + blockgr;    // Expand to include the full block
    } 
    else {
        nextpart[1] = x + 2;                 // Start of first sub-block in next partition
        nextpart[2] = x + einsanz + 1;       // Start of second sub-block in next partition
        nextpart[0] = 2;                     // Two sub-blocks in the partition
        part[0][x + 1] = x + einsanz;        // Mark end of first sub-block
    }

    // Loop through remaining blocks in the partition
    for (i = 2; i <= blanz; i++) {
        einsanz = einsen[x][i];                   // Number of special elements in the current block
        blockgr = block[i + 1] - block[i];        // Calculate the current block size

        if (einsanz == blockgr || einsanz == 0) {
            nextpart[++nextpart[0]] = block[i];   // Single block added
        } 
        else {
            nextpart[++nextpart[0]] = block[i];             // Start of sub-block with special elements
            nextpart[++nextpart[0]] = block[i] + einsanz;   // Split the block by special elements
        }
    }

    nextpart[nextpart[0] + 1] = n + 1;  // Set the final marker for partition end
}



/*
 ordrek does the insertion
 the edges, if necessary calls Girth-
 and Maxtest on, controls it
 Output. Transfer parameters:
 (mx,my) last entered. Edge
 vm min. nodes with grad=0
 before inserting (mx,my)
 tw waist size before insertion
 from (mx,my). tw=0, if still
 no circle exists
 lblock Block of part[mx], where
 was used
*/
static void ordrek(int mx, int my, int vm, int tw, int lblock) {
    int i;

    // Checks if there's enough space to add an edge from node `mx` to achieve the required degree.
    if (my > n - k && grad[mx] < k && n - my < k - grad[mx])   
        return;

    // Checks for space in the row if `mx` is close to reaching the max degree `k`.
    if (mx >= n - k && grad[mx] == k) {
        for (i = my + 1; i <= n; i++) {
            if (n - mx - 1 < k - grad[i]) 
                return;
        }
    }

    // `my < vm` indicates the need to check for new cycles in the graph.
    if (my < vm) {
        // Checks girth conditions
        if (tw > 3) {
            if (girthcheck2(mx, my, tw) < tw)
                return;
        } else if (tw == 0) {  // First cycle detection
            tw = girthstart();  
            if (tw == 0)
                return;
        }
    }

    // Checks if the row `mx` is fully populated (max degree).
    while (mx < n && grad[mx] == k) {
        semiverf(mx++);         // Refines partition for the next iteration
        lblock = 1;

#ifdef JOBS
        if (mx == mid_max || mx == splitlevel) {
            if (maxstartneu1(vm - 1) == 0)
                return;
            if (mx == splitlevel)
                if (++calls % jobs != jobNumber)
                    return;
        }
#else
        if (mx == mid_max) {
            if (maxstartneu1(vm - 1) == 0)
                return;
        }
#endif
    }

    if (vm <= my) vm = my + 1; // Ensures `vm` points to the minimum degree node with degree 0

    // Checks for regularity
    if (mx == n && grad[n] == k) {
        if (maxstartneu(tw)) {
            anz++;  // Counts generated graphs

#ifdef STAB
            if (print_all) {
                fprintf(autfile, "\nGraph %ld:\n", anz);
                nachblist();
                fprintf(autfile, "Taillenweite: %d\n", tw);
                stabprint();
            } else if (to_print > 0) {
                fprintf(autfile, "\nGraph %ld:\n", anz);
                nachblist();
                fprintf(autfile, "Taillenweite: %d\n", tw);
                stabprint();
                if (--to_print == 0) {
                    fclose(autfile);
                    fprintf(resultFile, "          %ld Graphen erzeugt\r", anz);
                    fflush(resultFile);
                    fseek(resultFile, fpos, 0);
                }
            }
#endif

#ifdef LIST
            if (store_all)
                komprtofile();
            else if (to_store > 0) {
                komprtofile();
                if (--to_store == 0) {
                    fclose(lstfile);
                    fprintf(resultFile, "          %ld Graphen erzeugt\r", anz);
                    fflush(resultFile);
                    fseek(resultFile, fpos, 0);
                }
            }

            if (count) {
                if (anz % dez == 0) {
                    fprintf(resultFile, "          %ld Graph generated\r", anz);
                    fflush(resultFile);
                    fseek(resultFile, fpos, 0);
                    if (anz % (dez * count) == 0)
                        dez *= 10;
                }
            }
#endif

            return;
        }
        jump = 1;  // Activates a learning effect for optimizations
        return;
    }

    // Edge insertion loop over available partitions
    for (i = lblock; i <= part[mx][0]; i++) {
        if ((my = part[mx][i] + einsen[mx][i]) < part[mx][i + 1]) {
            if (grad[my] < k && my <= vm) {
                // Insert edge (mx, my)
                g[mx][my] = g[my][mx] = 1;
                l[mx][++grad[mx]] = my;
                l[my][++grad[my]] = mx;
                einsen[mx][i]++;
                lgrad[my]++;

                // Recursive call
                ordrek(mx, my, vm, tw, i);

                // Remove edge (mx, my) and reset states
                g[mx][my] = g[my][mx] = 0;
                l[mx][grad[mx]] = 0;
                l[my][grad[my]] = 0;
                grad[mx]--;
                grad[my]--;
                lgrad[my]--;
                einsen[mx][i]--;

                // Additional constraints check
                if (mx >= n - k && n - mx - 1 < k - grad[my])
                    return;

                if (jump) {
                    if (g[i_1][j_1] == 0)
                        jump = 0;
                    else
                        return;
                }
            }
        }
    }
    return;
}



/*
 ordstart initializes the necessary data structures:

 g contains the adjacency matrix, meaning
 g[i][j] = 1 if i and j are connected, = 0 otherwise.

 l contains the neighborhood list, i.e.,
 l[i][1], ..., l[i][k] are the neighbors of i.

 part[i] contains the block structure based on
 the entries in row i-1 and part[i-1].
 Row i must be filled so that, within the
 blocks of part[i], the ones are on the left
 and the zeros on the right.
 part[i][0] : number of blocks;
 part[i][j] : start of the j-th block
              exception: if j = part[i][0], then n + 1;
 part[0][i] : end of the first block of row i before
              possibly splitting off a block of ones.

 einsen[i] contains the number of ones in row i
 with respect to the block structure in part[i].
 einsen[i][j] : number of ones in block j.

 lgrad[i] contains the number of ones
 in row i to the left of the diagonal.
*/


void ordstart(_n,_k,_t,_mid_max,
	      _splitlevel,_jobs,_jobnr,
	      _lstfile,_autfile,_resultFile,
	      _to_store,_to_print,_count,
	      _store_all,_print_all,_anz)

int _n,_k,_t,_mid_max,_splitlevel;
unsigned int _jobs,_jobnr;
FILE *_lstfile,*_autfile,*_resultFile;
unsigned long _to_print,_to_store,_count;
int _store_all,_print_all;
unsigned long *_anz;
{
 int m,in=0,zu=1;
 int  h,i,j;

 n = _n; k = _k; t = _t; mid_max = _mid_max;
 splitlevel = _splitlevel; jobs = _jobs; jobNumber = _jobnr;
 lstfile = _lstfile; autfile = _autfile; resultFile = _resultFile;
 to_print = _to_print; to_store = _to_store; count = _count;
 store_all = _store_all; print_all = _print_all;
 jump = girth_exact = calls = 0;
 dez = 1;
 anz = (*_anz);
 fpos = ftell(resultFile);

 if(k >= n)
    return;
 if(k == 0 || k == 1)
    return;
 if(n % 2 == 1 && k % 2 == 1)
    return;

 if(!(g = (int**)calloc(n + 1, sizeof(int*)))) err();
 if(!(l = (int**)calloc(n+1,sizeof(int*)))) err();
 if(!(zbk = (int**)calloc(n+1,sizeof(int*)))) err();
 if(!(transp = (int**)calloc(n+1,sizeof(int*)))) err();
 if(!(einsen=(int**)calloc(n+1,sizeof(int*)))) err();
 if(!(part  =(int**)calloc(n+1,sizeof(int*)))) err();
 if(!(kmn   =(int*)calloc(n+1,sizeof(int)))) err();
 if(!(grad  =(int*)calloc(n+1,sizeof(int)))) err();
 if(!(lgrad =(int*)calloc(n+1,sizeof(int)))) err();

 for(i=0;i<=n;i++)
    {
     if(!(g[i]     =(int*)calloc(n+1,sizeof(int))))err();
     if(!(l[i]     =(int*)calloc(k+1,sizeof(int))))err();
     if(!(transp[i]=(int*)calloc(n+1,sizeof(int))))err();
     if(!(einsen[i]=(int*)calloc(n+1,sizeof(int))))err();
     if(!(part[i]  =(int*)calloc(n+1,sizeof(int))))err();
     if(!(zbk[i]   =(int*)calloc(n+1+k*2,sizeof(int))))err();
    }

#ifdef STAB
 merz=n*(n-1)/2+1; /*n+(n-2)*k wuerde genuegen*/
 if(!(aut=(int**)calloc(merz+1,sizeof(int*))))err();
 for(i=1;i<=merz;i++)
     if(!(aut[i]=(int*)calloc(n+1,sizeof(int))))err();
#endif

#ifdef SHORTCODE
 if(!(lastcode = (int*)calloc(n * k /2 + 1,sizeof(int*))))err();
#endif

 for(i = 1;i <= n;i++)
     kmn[i]=i;

 part[0][1]=n;
 part[1][0]=1;
 part[1][1]=2;
 part[1][2]=n+1;
 einsen[1][1]=k;

 for(i = 3; i <= t;i++)
    {
     in += zu;
     if(i % 2 == 0){
	  zu *= (k-1);
      }
    }

 if(in*(k-1)+2>n)
    return;

 for(j=2;j<=k+1;j++)
    {
     l[1][++grad[1]]=j;
     l[j][++grad[j]]=1;
     g[1][j]=g[j][1]=1;
     lgrad[j]++;
    }

 for(i=2;i<=in;i++)
    {
     semiverf(i-1);
     for(h=1;h<k;h++)
	{
	 l[i][++grad[i]]=j;
	 l[j][++grad[j]]=i;
	 g[i][j]=g[j][i]=1;
	 lgrad[j++]++;
	}
     einsen[i][part[i][0]]=k-1;
    }

 f_1 = i; f_0 = j;
 m=(--j);--i;

 if(girth_exact)
   {
    semiverf(i);
    i=in+1;j=in+zu+1;h=1;
    l[i][++grad[i]]=j;
    l[j][++grad[j]]=i;
    g[i][j]=g[j][i]=1;
    lgrad[j]++;
    while(part[i][h]!=j)h++;
    einsen[i][h]++;
   }

 ordrek(i,j,m,0,1);

#ifdef STAB
 for(i=1;i<=merz;i++)
     free(aut[i]);
 free(aut);
#endif

#ifdef SHORTCODE
 free(lastcode);
#endif

 for(i=0;i<=n;i++)
    {
     free(g[i]);
     free(l[i]);
     free(zbk[i]);
     free(transp[i]);
     free(einsen[i]);
     free(part[i]);
    }

 free(g);
 free(l);
 free(kmn);
 free(zbk);
 free(part);
 free(grad);
 free(lgrad);
 free(einsen);
 free(transp);

 *_anz = anz;
 return;
}