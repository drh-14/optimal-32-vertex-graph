/*
 ============================================================================
 Name        : GenRegOMP.c
 Author      : Markus Meringer
 Version     : 1.1
 Copyright   : GNU General Public License
 Description : Simple OpenMP wrapper to GenReg

 List of Changes
 Author Date       Description
-----------------------------------------------------------------------------
 Markus	29.12.2020 Changed types of variables for counting from unsigned long
                   to unsigned long long, added comment on how to set shell
                   variables on Windows
 ============================================================================
 */

/**
 * GenRegOMP is a simple OpenMP wrapper to GenReg written in C.
 * It accepts 4-5 arguments: number of nodes, degree, girth,
 * number of tasks (i.e. the number jobs to split the generation task)
 * number of threads (i.e. the number of cores to be used).
 * The latter argument defines the maximum number of GenReg tasks,
 * which GenRegOMP starts in parallel.
 * GenReg writes erg files with the number of graphs generated
 * by each task, and finally the wrapper reads these files and
 * accumulates the numbers of graphs.
 *
 * Precondition: the GenReg must be in the path or the location
 * of the executable must be defined by an environment variable
 * named GENREG_EXE
 *
 * Setting environment variables in UNIX-like OS is generally well
 * known; in Windows PowerShell, use e.g.
 * $Env:GENREG_EXE += "C:\Programs\GenReg\GenReg.exe"
 * in Windows MSYS/MSYS2, which offers a Bourne shell, use e.g-
 * export GENREG_EXE="c/Programs/GenReg/GenReg.exe"
 *
 * Compiling: gcc -fopenmp GenRegOMP.c -o GenRegOMP.exe
 *
 * Example: GenRegOMP 18 3 3 8 4
 * will generate the following output:
 * nodes 18 degree 3 girth 3 tasks 8 threads 4
 * starting GenReg 18 3 3 -e -m 1 8
 * starting GenReg 18 3 3 -e -m 2 8
 * starting GenReg 18 3 3 -e -m 3 8
 * starting GenReg 18 3 3 -e -m 4 8
 * starting GenReg 18 3 3 -e -m 5 8
 * starting GenReg 18 3 3 -e -m 6 8
 * starting GenReg 18 3 3 -e -m 7 8
 * starting GenReg 18 3 3 -e -m 8 8
 * Completed
 * task 1: 4894 graphs
 * task 2: 6388 graphs
 * task 3: 6125 graphs
 * task 4: 4410 graphs
 * task 5: 4466 graphs
 * task 6: 4684 graphs
 * task 7: 4939 graphs
 * task 8: 5395 graphs
 * 41301 total graphs generated
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024

void CallGenReg(int iNodes, int iDegree, int iGirth, int iTask, int iNumTasks)
{
	// get the value of environment variable that points to the path of the GenReg executable
	const char* pGenRegExe = getenv("GENREG_EXE");

	// compose the command to call GenReg and print it to the console
	char pBuffer [BUFSIZE];
	sprintf(pBuffer,"%s %d %d %d -e -m %d %d", pGenRegExe ? pGenRegExe : "GenReg",
			iNodes, iDegree, iGirth, iTask+1, iNumTasks);
	printf("starting %s\n",pBuffer);

	// start GenReg
	system(pBuffer);
}

int main (int argc, char *argv[])
{
	// first parse command line an set up program parameters
	if(argc<5)
	{
		printf("Error: at least 4 arguments needed (nodes degree girth tasks [threads])\n");
		return 1;
	}

	int iNodes=atoi(argv[1]), iDegree=atoi(argv[2]), iGirth=atoi(argv[3]);
	int iNumTasks=atoi(argv[4]), iTask;
	int iNumThreads= argc>5 ? atoi(argv[5]) : omp_get_max_threads(); // the latter returns the number of cores

	printf("nodes %d degree %d girth %d tasks %d threads %d\n",iNodes,iDegree,iGirth,iNumTasks,iNumThreads);

	// now the parallel processing is initialized and executed
	omp_set_num_threads(iNumThreads);
	#pragma omp parallel for schedule(dynamic)
	for(iTask= 0;iTask<iNumTasks;iTask++)
		CallGenReg(iNodes, iDegree, iGirth, iTask, iNumTasks);
	printf("Completed\n");

	// finally read the result files
	char pBuffer [BUFSIZE];
	unsigned long long iNumGraphs, iTotalGraphs = 0;
	for(iTask=0; iTask<iNumTasks; iTask++)
	{
		sprintf(pBuffer,"%d%d_%d_%d#%d.erg",iNodes/10,iNodes%10,iDegree,3,iTask+1);
		FILE* pFile=fopen(pBuffer,"r");
		if(!pFile)
		{
			printf("Error opening %s\n",pBuffer);
			return 1;
		}
		for(int iLine=0; iLine<4; iLine++)
		{
			fgets(pBuffer, sizeof(pBuffer), pFile); // skip top lines
		}
		fscanf(pFile, "%I64u", &iNumGraphs);
		printf("task %d: %I64u graphs\n",iTask + 1,iNumGraphs);
		iTotalGraphs += iNumGraphs;
	}
	printf("%I64u total graphs generated\n",iTotalGraphs);
	return 0;
}
