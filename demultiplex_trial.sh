#!/bin/bash
#SBATCH -N 1
#SBATCH -n 1
#SBATCH --nodelist=c38
#SBATCH --nice=100000
#SBATCH --mail-type=END
#SBATCH --mail-user=jiayuel@mit.edu

# Make sure to complile the c++ first in the Scripts Directory
# g++ -O3 -o SHARE_demult_fastq_BC_AC_011525.o SHARE_demult_fastq_BC_AC.cpp -lgzstream -I/home/jiayuel/.local/usr/include -L/home/jiayuel/.local/lib -I/home/jiayuel/.local/usr/include -std=gnu++11 -lz -Wall
# then take the directory back to where this script was located at

# Create a space for debug if it doesn't exist 
# mkdir /home/jiayuel/data/easySHAREseq_workstation/Debug/Demultiplex_trial/input

module load miniconda3/v4

source /home/software/conda/miniconda3/bin/condainit

conda activate shareseq_env

echo "Working directory: $(pwd)"

# Main directory for your project
main_dir="/home/jiayuel/data/easySHAREseq_workstation/easySHARE_seq/Demultiplexing"
echo "Main directory: $main_dir"


# Input directory
input_dir="/home/jiayuel/data/easySHAREseq_workstation/Debug/Demultiplex_trial/input/"
echo "Input directory: $input_dir"
echo "Listing files in the input directory:"
ls -l $input_dir

# Output directory
output_dir="/home/jiayuel/data/easySHAREseq_workstation/Debug/Demultiplex_trial/output"
echo "Output directory: $output_dir"

# cp the test files
# make sure to double check if the directory is right.
#cp /home/jiayuel/data/30-1116768544/00_fastq/sgInert-1-sc*.fastq.gz

# Test run with a small batch of data (one sample in this case)
R1_file="${input_dir}sgInert-1-sc-ATAC_R1_001.fastq.gz"
R2_file="${input_dir}sgInert-1-sc-ATAC_R2_001.fastq.gz"
I1_file="${input_dir}sgInert-1-sc-RNA-seq_I1_001.fastq.gz"

# Verify that the files exist before proceeding
if [[ ! -f "$R1_file" ]] || [[ ! -f "$R2_file" ]] || [[ ! -f "$I1_file" ]] || [[ ! -f "$I2_file" ]]; then
    echo "Error: One or more required input files are missing in the input directory!"
    echo "Required files: $R1_file, $R2_file, $I1_file, $I2_file"
    exit 1
fi

echo "All required input files found!"

# copy barcodes and scripts from maindir
cp $main_dir/Scripts/BC_A.txt ./
cp $main_dir/Scripts/BC_C.txt ./
cp $main_dir/Scripts/SHARE_demult_fastq_BC_AC_011525.o ./


# Run the C++ executable with input and output directory paths as arguments
echo "Running the C++ executable SHARE_demult_fastq_orig.o with input and output directories"
./SHARE_demult_fastq_BC_AC_011525.o "$input_dir" "$output_dir"

# Output completion message
echo "Demultiplexing process completed."