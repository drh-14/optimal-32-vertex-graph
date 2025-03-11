#!/bin/bash
#SBATCH --job-name=DS_run
#SBATCH --output=DS_run_%j.out
#SBATCH --error=DS_run_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=96                     # Adjust number of MPI tasks as needed
#SBATCH --time=168:00:00                 # Adjust time limit as needed (Currently et to Max time for this partition)
#SBATCH --partition=extended-96core  # Change to your cluster's partition if required

# Set DS parameters: process jobs from START_JOB to END_JOB out of TOTAL_JOBS partitions.
START_JOB=1             # MODIFY ACCORDINGLY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
END_JOB=250             # MODIFY ACCORDINGLY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
TOTAL_JOBS=300000       # Total 300k jobs: 

echo "Starting DS with jobs ${START_JOB} to ${END_JOB} out of ${TOTAL_JOBS}"

# Run the DS executable with MPI
mpirun -np ${SLURM_NTASKS} ./DS ${START_JOB} ${END_JOB} ${TOTAL_JOBS}

echo "DS run completed."
