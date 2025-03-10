#!/bin/bash
#SBATCH --job-name=DS_run
#SBATCH --output=DS_run_%j.out
#SBATCH --error=DS_run_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=96                     # Adjust number of MPI tasks as needed
#SBATCH --time=24:00:00                 # Adjust time limit as needed (Currently et to Max time for this partition)
#SBATCH --partition=long-96core-shared  # Change to your cluster's partition if required

# Set DS parameters: process jobs from START_JOB to END_JOB out of TOTAL_JOBS partitions.
START_JOB=1
END_JOB=250
TOTAL_JOBS=5000

echo "Starting DS with jobs ${START_JOB} to ${END_JOB} out of ${TOTAL_JOBS}"

# Run the DS executable with MPI
mpirun -np ${SLURM_NTASKS} ./DS ${START_JOB} ${END_JOB} ${TOTAL_JOBS}

echo "DS run completed."
