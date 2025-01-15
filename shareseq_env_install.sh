#!/bin/bash
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --nice=100000
#SBATCH --mail-type=END
#SBATCH --mail-user=jiayuel@mit.edu

module load miniconda3/v4

source /home/software/conda/miniconda3/bin/condainit

conda create --name shareseq_env

conda activate shareseq_env

conda config --add channels defaults
conda config --add channels bioconda
conda config --add channels conda-forge

# UMItools need a >=3.10 python
# Install required packages with specified versions
conda install -y bwa=0.7.17 samtools=1.16.1 STAR=2.7.9a cutadapt=4.0 picard=2.18.25 macs2=2.2.7.1

# featureCounts=2.0.1 UMItools=1.1.2 need a seperate environmemt due to conflict python version

# Capture the installed packages and their versions to a log file
echo "Installed packages and their versions:" > install_log.txt
conda list >> install_easySHAREseq_env_log.txt

# Check for successful installation
if [ $? -eq 0 ]; then
    echo "All packages installed successfully!"
else
    echo "Error in package installation." >&2
fi

conda install -c bioconda umi_tools=1.1.2