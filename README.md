
# EasySHARE-Seq Analysis

This document provides a step-by-step guide for setting up the **easySHARE_seq** analysis environment, compiling the C++ code, and running the test demultiplexing runs. Follow the instructions below to ensure proper installation and configuration on KI Luria computing cluster.

The original code credit refers to https://github.com/vosoltys/easySHARE_seq/tree/main.

---

## 1. Setup EasySHARE Seq Workstation

### 1.1 Create a an analysis folder
Create a directory called `easySHARE_seq_workstation` in your desired location:

```bash
mkdir easySHARE_seq_workstation
```

### 1.2 Copy the Bash Installation Script
Copy your `shareseq_env_install.sh` file to the `easySHARE_seq_workstation` directory:

```bash
cp /path/to/shareseq_env_install.sh ./easySHARE_seq_workstation/
```

---

## 2. Create a Conda Environment

### 2.1 Choose Idle Nodes for Installation
Before creating a Conda environment, use the `sinfo` command to select an idle node on the cluster:

```bash
sinfo
```

Identify an idle node (e.g., `<node_name>`), which will help prevent the installation from crashing. Replace `<node_name>` with the chosen node in the next step.

### 2.2 Install Conda Environment
Run the following command to execute the installation script on the chosen node. This ensures that the Conda environment installation happens on an idle node, avoiding resource contention.

```bash
sbatch -n 1 --nodelist=<node_name> ./shareseq_env_install.sh
```

---

## 3. Clone the EasySHARE Seq Repository

### 3.1 Clone the GitHub Repository
Clone the `easySHARE_seq` repository from GitHub to get the necessary code for the analysis:

```bash
git clone https://github.com/vosoltys/easySHARE_seq.git
```

### 3.2 Copy optimized C++ Code and compiled code (slurm compatible) for Demultiplexing 
Copy the optimized C++ code `SHARE_demult_fastq_BC_AC.cpp` into the appropriate directory. 
The file `SHARE_demult_fastq_BC_AC_011525.o` can be directly used in test runs without compilation if you don't want to make any changes to the .cpp file. 
The test run is facilitated by the provided `demultiplex_trial.sh` script. It will be explained in the next section. 


```bash
cp SHARE_demult_fastq_BC_AC.cpp ./easySHARE_seq_workstation/easySHAREseq/easySHARE_seq/Demultiplexing/Scripts/
cp SHARE_demult_fastq_BC_AC_011525.o ./easySHARE_seq_workstation/easySHAREseq/easySHARE_seq/Demultiplexing/Scripts/
```

**Note**: Comparing with the original one, this C++ code has been optimized for two rounds of barcode processing and includes additional functions for debugging and sanity checks.

---

### 3.3 (optional) Compile the C++ Code
Just in case changes to C++ code need to be made, compilation is required to run it. 

#### 3.4.1 clone gzstream repo
To compile the C++ code that requires the `gzstream` library, first clone the `gzstream` repository:

```bash
git clone https://github.com/kanedo/gzstream.git
```

#### 3.4.2 Modify the Makefile
Since we do not have `sudo` access on the server, we will modify the `Makefile` in the `gzstream` repository to install the library into your local directory.

1. Open the `Makefile` in the `gzstream` repository.
2. Find the `install` target and modify it as follows:

```make
install: libgzstream.a test
    mkdir -p ~/.local/lib/
    mkdir -p ~/.local/usr/include/
    cp libgzstream.a ~/.local/lib/
    cp gzstream.h ~/.local/usr/include/
    chmod o+r ~/.local/usr/include/gzstream.h
```

#### 3.4.3 Install gzstream
Once the `Makefile` has been modified, run the following command to install the `gzstream` library:

```bash
make install
```

This will install the `gzstream` library into your local `~/.local` directory, which does not require `sudo` access.

---

#### 3.4.4  Compile the C++ Code

Activate the Conda environment to ensure that the necessary dependencies are available for compilation:

```bash
module load miniconda3/v4

source /home/software/conda/miniconda3/bin/condainit

conda activate shareseq_env
```

Once the Conda environment is activated, compile the `SHARE_demult_fastq.cpp` script with the following command. Make sure you update the path according to your user directory and include the necessary flags for gzstream:

```bash
g++ -O3 -o SHARE_demult_fastq.o SHARE_demult_fastq.cpp -lgzstream -I/home/username/.local/usr/include -L/home/username/.local/lib -I/home/username/.local/usr/include -std=gnu++11 -lz -Wall
```

Replace `username` with your actual luria username and adjust the paths accordingly.

---

## 4. Demultiplex Test Runs

### 4.1 Create a Folder for Test Runs
Create a folder called `Debug` within the `easySHARE_seq_workstation` directory. Inside `Debug`, create a subfolder called `Demultiplex_trial` to hold the test data:

```bash
mkdir -p easySHARE_seq_workstation/Debug/Demultiplex_trial
```

### 4.2 Copy Sample Data
Ensure that the sample data required for testing is copied to the `Demultiplex_trial` directory. Follow the instructions provided in the `demultiplex_trial.sh` script for proper setup.

### 4.3 Running the Demultiplexing Script
Run the demultiplexing script on the test data. Ensure that the correct input and output paths are set in the script and that the `shareseq_env` Conda environment is activated.

