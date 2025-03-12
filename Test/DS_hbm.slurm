#!/bin/bash
#SBATCH --job-name=DS_run
#SBATCH --output=DS_run_%j.out
#SBATCH --error=DS_run_%j.err
#SBATCH --nodes=2
#SBATCH --ntasks=96                     # Adjust number of MPI tasks as needed
#SBATCH --time=168:00:00                 # Adjust time limit as needed (max for this partition)
#SBATCH --partition=hbm-extended-96core   # Use the HBM partition

# DS parameters: process jobs from START_JOB to END_JOB out of TOTAL_JOBS partitions.
START_JOB=4001             # MODIFY AS NEEDED
END_JOB=5000               # MODIFY AS NEEDED
TOTAL_JOBS=6000            # Total number of partitions/jobs (6k works fine)

# Set environment variables for UCX:
# Instead of using mlx5_2 (which appears unavailable), we set UCX_NET_DEVICES to an interface reported as available.
# According to the error message, valid alternatives include 'ib0' (TCP) or 'mlx5_0:1' (IB).
# Here we choose ib0.
export UCX_NET_DEVICES=ib0
export UCX_WARN_UNUSED_ENV_VARS=n
# Do not force TCP transport explicitly (i.e. do not set UCX_TLS)

echo "Starting DS with jobs ${START_JOB} to ${END_JOB} out of ${TOTAL_JOBS}"

# Launch the DS executable via MPI
mpirun -np ${SLURM_NTASKS} ./DS ${START_JOB} ${END_JOB} ${TOTAL_JOBS}

echo "DS run completed."

