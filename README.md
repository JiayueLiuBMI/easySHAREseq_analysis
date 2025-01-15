Here is the complete `README.md` markdown file for your project setup:

```markdown
# EasySHARE Seq Workstation Setup

This document provides a step-by-step guide for setting up the **easySHARE_seq** analysis environment, compiling the C++ code, and running the test demultiplexing runs. Follow the instructions below to ensure proper installation and configuration.

---

## 1. Setup EasySHARE Seq Workstation

### 1.1 Create a Folder for the Workstation
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

### 3.2 Copy C++ Code for Demultiplexing
Copy the optimized C++ code `SHARE_demult_fastq_BC_AC.cpp` into the appropriate directory:

```bash
cp SHARE_demult_fastq_BC_AC.cpp ./easySHARE_seq_workstation/easySHAREseq/easySHARE_seq/Demultiplexing/Scripts/
```

**Note**: This C++ code has been optimized for two rounds of barcode processing and includes additional functions for debugging and sanity checks.

### 3.3 Copy Precompiled C++ Object File
Copy the precompiled object file `SHARE_demult_fastq_BC_AC_011525.o` into the directory for test runs:

```bash
cp SHARE_demult_fastq_BC_AC_011525.o ./easySHARE_seq_workstation/easySHAREseq/easySHARE_seq/Demultiplexing/Scripts/
```

The test run is facilitated by the provided `demultiplex_trial.sh` script.

---

## 4. Install gzstream

### 4.1 Clone gzstream Repository
To compile the C++ code that requires the `gzstream` library, first clone the `gzstream` repository:

```bash
git clone https://github.com/kanedo/gzstream.git
```

### 4.2 Modify the Makefile
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

### 4.3 Install gzstream
Once the `Makefile` has been modified, run the following command to install the `gzstream` library:

```bash
make install
```

This will install the `gzstream` library into your local `~/.local` directory, which does not require `sudo` access.

---

## 5. Compile the C++ Code

### 5.1 Activate the Conda Environment
Activate the Conda environment to ensure that the necessary dependencies are available for compilation:

```bash
module load miniconda3/v4

source /home/software/conda/miniconda3/bin/condainit

conda activate shareseq_env
```

### 5.2 Compile the C++ Code
Once the Conda environment is activated, compile the `SHARE_demult_fastq.cpp` script with the following command. Make sure you update the path according to your user directory and include the necessary flags for gzstream:

```bash
g++ -O3 -o SHARE_demult_fastq.o SHARE_demult_fastq.cpp -lgzstream -I/home/jiayuel/.local/usr/include -L/home/jiayuel/.local/lib -I/home/jiayuel/.local/usr/include -std=gnu++11 -lz -Wall
```

Replace `jiayuel` with your actual username and adjust the paths accordingly.

---

## 6. Demultiplex Test Runs

### 6.1 Create a Folder for Test Runs
Create a folder called `Debug` within the `easySHARE_seq_workstation` directory. Inside `Debug`, create a subfolder called `Demultiplex_trial` to hold the test data:

```bash
mkdir -p easySHARE_seq_workstation/Debug/Demultiplex_trial
```

### 6.2 Copy Sample Data
Ensure that the sample data required for testing is copied to the `Demultiplex_trial` directory. Follow the instructions provided in the `demultiplex_trial.sh` script for proper setup.

### 6.3 Running the Demultiplexing Script
Run the demultiplexing script on the test data. Ensure that the correct input and output paths are set in the script and that the `shareseq_env` Conda environment is activated.

---

## Additional Notes

- **Debugging**: The provided C++ code (`SHARE_demult_fastq_BC_AC.cpp`) includes additional debugging and sanity check functions to help ensure the integrity of the demultiplexing process.
- **Sample Data**: Make sure to have sample input data in the correct format as expected by the script. Consult the provided `demultiplex_trial.sh` script for details on the input file structure and data.
- **Compiling Dependencies**: The compilation process requires the `gzstream` library, which is used to handle compressed FASTQ files. Follow the instructions above to install and configure `gzstream`.

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
```

### Notes:
- Make sure to replace `/path/to/shareseq_env_install.sh` with the actual path to your script.
- The instructions are tailored to the typical setup for a high-performance cluster environment, where tools like `sinfo` and `sbatch` are used for job scheduling.
- Adjust user-specific paths such as `jiayuel` in the compilation commands to match your own system.
