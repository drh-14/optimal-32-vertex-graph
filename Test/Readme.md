Dynamic Scheduler (DS) & GENREG Testing Instructions

This document provides instructions to test the Dynamic Scheduler (DS) and GENREG on Seawulf.

1. Import Files

Copy the DS and GENREG files into the Test folder on Seawulf.

2. Build the Executables

Open a terminal in the Test folder and run:

make

This will compile all necessary executables.

3. Load Required Modules

Depending on the node you are using, load the following modules:

For Milan 1 & 2 and XeonMax Nodes:

module load mpich/gcc/3.4.2 gcc/11.2.0 slurm

For Login1 & Login2 Nodes:

module load mpich/gcc/3.2.1 gcc/10.2.0 slurm

3.1 Complie DS.c before submitting the job

mpicc -O -o DS DS.c

4. Configure the Slurm Job

Edit the Slurm script DS.slurm to set the desired job parameters. In particular, adjust the start and end job numbers (these are inclusive) and ensure that the total jobs parameter is consistent (300,000k).

To edit the file, run:

nano DS.slurm

Make your changes and save the file.

5. Submit the Slurm Job

Once you have configured DS.slurm, submit it with:

sbatch DS.slurm

6. Monitor Job Status

To check the status of your jobs, run:

squeue -u $USER

_____________________________________________________________________________________________________________________________

KEY CHANGES in genreg.c and CSV Merging Strategy

1. ASPL Computation Integration
   
- New Function Added:
A function called compute_aspl(void) was added. This function calculates the Average Shortest Path Length (ASPL) for a given graph using the Floyd–Warshall algorithm.

2. Local Best Graph Storage Using Linked List
   
- Dynamic Linked List:
Based on the ASPL computed for each graph, the code now decides whether to “trash” (ignore) or “append” the graph to a local linked list.

- Decision Process:
1) When a graph is generated, its ASPL is computed.
2) If the ASPL is lower than the current best (minASPL) on that node (using a small tolerance of 1e-6 to account for floating-point imprecision), the node flushes its local list and stores this new graph.
3) If the ASPL is equal (within the tolerance) to the current best, the graph is appended to the list.
4) Graphs with a higher ASPL than the current best are ignored.

- Output File Naming:
Once local enumeration is complete, each node writes the adjacency lists of its best graphs into a CSV file. The file is named in the format <i>_<total_job_number>.csv, where <i> is the 1-based partition (or job) number and <total_job_number> is the total number of partitions (jobs) specified.

3. CSV Output and Merging Strategy
   
- Separate Output Files:
Rather than merging all best graphs during enumeration, each node now writes its results to a separate CSV file.

- Post-Processing with Python:
After all jobs are complete, a Python script (merge.py) is used to merge the individual CSV files into one consolidated CSV file.

- Merging Logic:
1) The Python script reads each CSV file (named according to the partition number).
2) It parses the ASPL value (found on the second line of each graph block).
3) While merging, it only includes graphs if their ASPL is equal to or lower than the current global minimum ASPL.
4) This approach avoids the complexity and potential bugs of maintaining a global minimum in a distributed environment.

- Efficiency:
Batch tests show that merging 5000 CSV files takes only a few minutes, which is negligible compared to the enumeration time.

4. Why These Changes?

- Modular & Robust:
Keeping the CSV output separate and merging them later minimizes complexity in the distributed enumeration phase. It reduces the risk of errors that might occur when trying to manage a global state across nodes.

- Ease of Debugging:
With separate output files and a dedicated merging script, it is easier to diagnose issues since each node’s output is isolated.

- Performance Considerations:
The post-processing merge is very lightweight compared to the graph enumeration work.
