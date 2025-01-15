/*                                          
 * variant_file.cpp
 *
 *  Created on: Jan 28, 2025
 *      Authors: Jiayue Liu
 *               Andreea Dreau, https://github.com/adreau
 *               Frank Chan, https://github.com/evolgenomics
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <gzstream.h>

using namespace std;

// Barcode file names for A and C
string barcode_A = "BC_A.txt";
string barcode_C = "BC_C.txt";

// Maps to store barcodes for A and C
map<string, string> bc_A;
map<string, string> bc_C;

typedef pair<unsigned int, unsigned int> pair_int;

// Function to compute the Levenshtein distance (edit distance)
unsigned int edit_distance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    vector<std::vector<unsigned int>> d(len1 + 1, vector<unsigned int>(len2 + 1));

    d[0][0] = 0;
    for (unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
    for (unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

    for (unsigned int i = 1; i <= len1; ++i)
        for (unsigned int j = 1; j <= len2; ++j)
            d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));

    return d[len1][len2];
}

// Find the barcode with the minimum edit distance
string min_edit_distance(const std::string& s1, map<string, string> bc, string code_letter) {
    int min = 10000;
    string code_min_dist;
    string code_str;
    int ed, occ = 0;
    map<string, string>::iterator it;
    for (it = bc.begin(); it != bc.end(); it++) {
        ed = edit_distance(s1, it->first);
        if (min > ed) {
            min = ed;
            code_min_dist = it->second;
            code_str = it->first;
            occ = 1;
        } else if (min == ed) {
            occ++;
        }
    }

    if (occ > 1) code_min_dist = code_letter + "000";
    return code_min_dist;
}

// Load barcodes into the map
void load_barcodes(map<string, string>& bc_list, string file) {
    ifstream barcode_file(file.c_str());
    string line;
    while (getline(barcode_file, line)) {
        bc_list.insert(std::pair<string, string>(line.substr(5), line.substr(0, 4)));
    }
}



// Load all barcodes (A and C)
void load_all_barcodes() {
    load_barcodes(bc_A, barcode_A);
    load_barcodes(bc_C, barcode_C);

    // Debugging: print the contents of the barcode maps
    cout << "Loaded barcodes A: " << bc_A.size() << ", C: " << bc_C.size() << endl;
    for (auto& pair : bc_A) {
        cout << "A Barcode: " << pair.first << " -> " << pair.second << endl;
    }
    for (auto& pair : bc_C) {
        cout << "C Barcode: " << pair.first << " -> " << pair.second << endl;
    }
    // Output should be like: 
    //Loaded barcodes A: 192, C: 192
    //A Barcode: AACCACA -> A151
    //A Barcode: AACGGAG -> A056 ...
}


// Function to check if the input files can be opened
bool check_file_existence(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file " << filename << endl;
        return false;
    }
    return true;
}


// Function to get the barcode codes (A and C) from the I1 file
void getCode(igzstream& I1, string& codeA, string& codeC,
    string& RX1, string& QX1, string& read_type1,
    int code_total_length, string code_letter1, string code_letter2,
    map<string, string> bc1, map<string, string> bc2) {

    read_type1 = "correct";
    string line;
    string codeA_inFile, codeC_inFile;
    map<string, string>::iterator a;
    map<string, string>::iterator c;

    for (int i = 0; i < 2; i++) getline(I1, line);  // skip first two lines
    RX1 = line;

    if (line.length() < code_total_length) {
        // If the barcode sequence length is smaller than the expected code_total_length, 
        // It assigns "default" values (codeA and codeC) indicating invalid or unclear barcodes.
        codeA = code_letter1 + "000";
        codeC = code_letter2 + "000";
    } else {
        // Barcode A
        codeA_inFile = line.substr(10);
        a = bc1.find(codeA_inFile);
        if (a == bc1.end()) {
            codeA = min_edit_distance(codeA_inFile, bc1, code_letter1);
            read_type1 = "corrected";
        } else {
            codeA = a->second;
        }

        // Barcode C
        codeC_inFile = line.substr(0, 7);
        c = bc2.find(codeC_inFile);
        if (c == bc2.end()) {
            codeC = min_edit_distance(codeC_inFile, bc2, code_letter2);
            read_type1 = "corrected";
        } else {
            codeC = c->second;
        }
    }

    if (codeA.compare(code_letter1 + "000") == 0 || codeC.compare(code_letter2 + "000") == 0) {
        read_type1 = "unclear";
    }

    // Debugging: Check what is happening with barcodes
    cout << "Read Type: " << read_type1 << " | Barcode A: " << codeA << " | Barcode C: " << codeC << endl;

    getline(I1, line);  // Skip sequence line
    getline(I1, line);  // Skip plus line
    QX1 = line;
}


int main(int argc, char* argv[]) {

    load_all_barcodes();
    cout << "Loaded barcodes: " << bc_A.size() << " A, " << bc_C.size() << " C" << endl;

    string path_to_reads = argv[1];
    string path_output = argv[2];

    string R1_file = path_to_reads + "R1_001.fastq.gz";
    string R2_file = path_to_reads + "R2_001.fastq.gz";
    string I1_file = path_to_reads + "I1_001.fastq.gz";
    
    string R1_outfile = path_output + "_R1_001.fastq.gz";
    string R2_outfile = path_output + "_R2_001.fastq.gz";

    string clearBC_logfile = path_output + "_clearBC.log";
    string unclearBC_logfile = path_output + "_unclearBC.log";


    // Check if input files exist
    if (!check_file_existence(R1_file) || !check_file_existence(R2_file) || !check_file_existence(I1_file)) {
        return 1;  // Exit if any file cannot be opened
    }

    cout << "Opening input files..." << endl;
    cout << "R1: " << R1_file << endl;
    cout << "R2: " << R2_file << endl;
    cout << "I1: " << I1_file << endl;


    igzstream R1(R1_file.c_str());
    igzstream R2(R2_file.c_str());
    igzstream I1(I1_file.c_str());


    ogzstream R1_out(R1_outfile.c_str());
    ogzstream R2_out(R2_outfile.c_str());

    ofstream clearBC_log(clearBC_logfile.c_str());
    ofstream unclearBC_log(unclearBC_logfile.c_str());

    string codeA, codeC;
    string RX1, RX2, QX1, QX2;
    string read_type1, read_type2;

    map<string, pair_int> clear_read_map;
    map<string, pair_int>::iterator it_clear;
    map<string, int> unclear_read_map;
    map<string, int>::iterator it_unclear;

    string line;
    string name;
    string code;
    int posName;

    while (getline(R1, line)) {
        // Get barcode A and C
        // pay attention to the 'code_total_length' parameter!!! 
        getCode(I1, codeA, codeC, RX1, QX1, read_type1, 17, "A", "C", bc_A, bc_C);

        // Process the read name and append the barcode info
        posName = line.find(" ");
        name = line.substr(0, posName + 1);

        // Append BX tag with both barcode A and C
        name = name.append("BX:Z:");
        name = name.append(codeA).append(codeC);

        code = codeA + codeC;

        // Append RX and QX tags
        name = name.append("\tRX:Z:").append(RX1).append("+").append(RX2);
        name = name.append("\tQX:Z:").append(QX1).append("+").append(QX2);

        // Write out the reads
        R1_out << name << endl;
        R2_out << name << endl;

        // Write out the sequence and quality score
        for (int i = 0; i < 3; i++) {
            getline(R1, line);
            R1_out << line << endl;
        }
        getline(R2, line);
        for (int i = 0; i < 3; i++) {
            getline(R2, line);
            R2_out << line << endl;
        }

        // Sort reads into clear vs unclear based on barcode status
        if (read_type1.compare("unclear") == 0) {
            it_unclear = unclear_read_map.find(code);
            if (it_unclear != unclear_read_map.end())
                it_unclear->second++;
            else
                cout << "Inserting into unclear map: " << code << endl;
                unclear_read_map.insert(pair<string, int>(code, 1));
        } else {
            it_clear = clear_read_map.find(code);
            if (read_type1.compare("corrected") == 0) {
                if (it_clear != clear_read_map.end())
                    it_clear->second.second++;
                else
                    cout << "Inserting into clear map (corrected): " << code << endl;
                    clear_read_map.insert(make_pair(code, make_pair(1, 1)));
            } else {
                if (it_clear != clear_read_map.end())
                    it_clear->second.first++;
                else
                    cout << "Inserting into clear map (correct): " << code << endl;
                    clear_read_map.insert(make_pair(code, make_pair(1, 0)));
            }
        }
    }
        
        
    // Log results for clear and unclear reads
    clearBC_log << "Barcode \t Correct reads \t Corrected reads" << endl;
    for (it_clear = clear_read_map.begin(); it_clear != clear_read_map.end(); ++it_clear) {
        clearBC_log << it_clear->first << "\t" << it_clear->second.first << "\t" << it_clear->second.second << endl;
    }

    unclearBC_log << "Barcode \t Unclear reads" << endl;
    for (it_unclear = unclear_read_map.begin(); it_unclear != unclear_read_map.end(); ++it_unclear) {
        unclearBC_log << it_unclear->first << "\t" << it_unclear->second << endl;
    }

    return 0;
}
