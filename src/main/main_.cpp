#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <random>
#include "main_vampire.cpp"
#include "store_outputs.cpp"
#include "vio.hpp"

// Structure to hold the limits
struct NeighborLimits {
    float lower_limit;
    float upper_limit;
};

// Structure to uniquely identify a combination
struct Key {
    int i, j, k;
    bool operator==(const Key& other) const {
        return i == other.i && j == other.j && k == other.k;
    }
};

// Custom hash function for Key
namespace std {
    template <>
    struct hash<Key> {
        std::size_t operator()(const Key& key) const {
            std::size_t h1 = std::hash<int>()(key.i);
            std::size_t h2 = std::hash<int>()(key.j);
            std::size_t h3 = std::hash<int>()(key.k);
            return h1 ^ (h2 << 1) ^ (h3 << 1); // Combine hash values
        }
    };
}

float generateRandomFloat(float lower_limit, float upper_limit) {
    // Random number generator with a random seed
    std::random_device rd;                  // Seed for the random number engine
    std::mt19937 gen(rd());                 // Mersenne Twister engine seeded with rd
    std::uniform_real_distribution<> dis(lower_limit, upper_limit);  // Uniform distribution between limits

    return dis(gen);  // Generate a random float in the range [lower_limit, upper_limit]
}

namespace m1{
    std::string line;
    int num_materials;
    int num_neighbors;
    int num_runs;
    std::string mat_file;
    std::unordered_map<Key, NeighborLimits> limits_map;
    void ml_file() {
        std::string filename = "ML";
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file " << filename << std::endl;
            
        }
        
        while (std::getline(file, line)) {
            if (line.find("mat_file=") != std::string::npos) {
                mat_file = (line.substr(line.find('=') + 1));

            }
            else if (line.find("num_materials=") != std::string::npos) {
                num_materials = std::stoi(line.substr(line.find('=') + 1));

            }
            else if (line.find("num_neighbors=") != std::string::npos) {
                num_neighbors = std::stoi(line.substr(line.find('=') + 1));

            }
            else if (line.find("num_runs=") != std::string::npos) {
                num_runs = std::stoi(line.substr(line.find('=') + 1));

            }
            
            else if (line.find("neighbors=[") != std::string::npos) {
                // Extract indices i, j, k from the line
                size_t start = line.find('[') + 1;
                size_t end = line.find(']');
                int i = std::stoi(line.substr(start, line.find(']', start) - start));
            
                start = line.find('[', end + 1) + 1;
                end = line.find(']', start);
                int j = std::stoi(line.substr(start, end - start));

                start = line.find('[', end + 1) + 1;
                end = line.find(']', start);
                int k = std::stoi(line.substr(start, end - start));

                // Extract the lower and upper limits
                start = line.find("neighbors=[") + 11; // 11 is the length of "neighbors=["

                // Find the position of the closing ']'
                end = line.find(']', start);

                // Extract the substring containing the limits
                std::string values = line.substr(start, end - start);

                // Now parse the values string to extract lower and upper limits
                std::stringstream ss(values);
                std::string item;
                std::vector<float> limits;
                while (std::getline(ss, item, ',')) {
                    limits.push_back(std::stof(item));
                }

                if (limits.size() == 2) {
                    Key key = {i, j, k};
                    NeighborLimits nl = {limits[0], limits[1]};
                    limits_map[key] = nl;
                }
            
            }
        }

        file.close();
    }
}

namespace m3{
        // Function to read file lines into a vector
    void deleteFileLines(int c) {
        std::cout<<m1::mat_file<<std::endl;
        std::ifstream file(m1::mat_file);
        std::vector<std::string> lines;
        std::string line;

        if (!file.is_open()) {
            std::cerr << "Error: Could not open the file!" << std::endl;
            return;
        }

        // Read all lines from the file
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        std::cout << "Number of lines in file: " << lines.size() << std::endl;

        if (c >= lines.size()) {
            std::cerr << "Error: The file has fewer lines than the number of lines to delete!" << std::endl;
            return;
        }
        lines.resize(lines.size() - c);
        std::cout << "Number of lines after in file: " << lines.size() << std::endl;

        file.close();

        std::ofstream outputFiles(m1::mat_file, std::ios::trunc);
        if (!outputFiles.is_open()) {
            std::cerr << "Error: Could not open the file for writing!" << std::endl;
            return;
        }

        for (const auto& remainingLine : lines) {
            if (!remainingLine.empty()) { // Check if the line is not empty
                outputFiles << remainingLine << std::endl;
            }
        }
        std::cout << " deleted " << std::endl;
        outputFiles.close();
        
    }
}


namespace m2{
    // Output the parsed data
    int matf(){
        std::cout << "mat_file = " << m1::mat_file << std::endl;
        std::ofstream outputFile(m1::mat_file, std::ios_base::app); // Open file in append mode
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open the output file" << std::endl;
        }

        std::cout << "num_materials = " << m1::num_materials << std::endl;
        std::cout << "num_neighbors = " << m1::num_neighbors << std::endl;
        std::cout << "num_runs = " << m1::num_runs << std::endl;

        // Output all [i][j][k] combinations with their limits
        std::cout << "Limits for each [i][j][k]:" << std::endl;
        int c=0;
        for (int i = 1; i <= m1::num_materials; ++i) {
            for (int j = 1; j <= m1::num_materials; ++j) {
                for (int k = 1; k <= m1::num_neighbors; ++k) {
                    Key key = {i, j, k};
                    if (m1::limits_map.find(key) != m1::limits_map.end()) {
                        NeighborLimits& limits = m1::limits_map[key];
                        float random_float = generateRandomFloat(limits.lower_limit, limits.upper_limit);
                        outputFile<<std::endl<<"material["<<i<<"]:exchange-matrix-"<<k<<"nn["<<j<<"]="<<random_float;
                        c=++c;
                        if (i!=j){
                            outputFile<<std::endl<<"material["<<j<<"]:exchange-matrix-"<<k<<"nn["<<i<<"]="<<random_float;
                            c=++c;
                        }
                    } else {
                        continue;
                    }
                }
            }
        }
        
        outputFile.close();
        std::cout << "printed"<<c << std::endl;
        return c;
    }
    
}

#include <cstdlib> // For EXIT_SUCCESS and EXIT_FAILURE

// int main(int argc, char* argv[])
// {   vmpi::initialise(argc, argv);
//     std::cout << "started" << std::endl;
//     m1::ml_file();

//     for (int run = 1; run <= m1::num_runs; ++run) {
//         if (run == 1) {
//             m2::matf();

//             // Call main_start and check its return value
//             if (main_start(argc, argv) == EXIT_SUCCESS) {
//                 copyFilesToFolder(run);
//                 std::cout << "completed- " << run << std::endl;
//             } else {
//                 std::cerr << "Error: main_start failed for run " << run << std::endl;
//                 // Handle the error as needed
//             }
//         } else {
//             int c = m2::matf();
//             m3::deleteFileLines(c);

//             // Call main_start and check its return value
//             if (main_start(argc, argv) == EXIT_SUCCESS) {
//                 copyFilesToFolder(run);
//                 std::cout << "completed- " << run << std::endl;
//             } else {
//                 std::cerr << "Error: main_start failed for run " << run << std::endl;
//                 // Handle the error as needed
//             }
//         }
//     }

//     std::cout << "end" << std::endl;
//     return 0;
// }

int main(int argc, char* argv[]){
    std::cout << "started" << std::endl;
    m1::ml_file();
//std::cout << "mmat_file: " << m1::mat_file << std::endl;
// std::cout << "mnum_runs: " << m1::num_runs << std::endl;
    int c;
    vmpi::initialise(argc, argv);
    for (int run = 1; run <= m1::num_runs; ++run) {
        // If num_runs is greater than 1, delete lines after each run
        
        if (run == 1) {
            m2::matf();
          //  m3::deleteFileLines(m2::c);
            main_start(argc,argv);
            copyFilesToFolder(run);
            std::cout << "completed- " << run << std::endl;
            vout::file_output_list.clear();
            vout::file_output_list.shrink_to_fit();
            vout::screen_output_list.clear();
            vout::screen_output_list.shrink_to_fit();
        }
        else{
            int c=m2::matf();
            std::cout << "c- " << c << std::endl;
            m3::deleteFileLines(c);
            m2::matf();
            main_start(argc,argv);
            copyFilesToFolder(run);
            std::cout << "completed- " << run << std::endl;
            vout::file_output_list.clear();
            vout::file_output_list.shrink_to_fit();
            vout::screen_output_list.clear();
            vout::screen_output_list.shrink_to_fit();
        
        }
    
      
    std::cout << "end" << std::endl;
} 
}
