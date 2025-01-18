#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <random>
#include "main_vampire.cpp"

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
    void ml_file(std::string& mat_file, int& num_runs) {
        std::string filename = "ML";
        std::ifstream file(filename);
    //   namespace fs = std::__fs::filesystem;

        // Get current working directory
    //  fs::path cwd = fs::current_path();
    //  std::cout << "Current working directory: " << cwd.string() << std::endl;

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
    void deleteFileLines(const std::string& filename, int c) {
        std::ifstream file(filename);
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
        lines.resize(lines.size() - c);
        file.close();

        std::ofstream outputFiles(filename, std::ios::trunc);
        if (!outputFiles.is_open()) {
            std::cerr << "Error: Could not open the file for writing!" << std::endl;
            return;
        }

        // Write the remaining lines back to the file
        for (const auto& remainingLine : lines) {
            outputFiles << remainingLine << std::endl;
        }

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
            return 1;
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
        std::cout << c << std::endl;
        outputFile.close();
        m3::deleteFileLines(m1::mat_file,c);
        // std::vector<std::string> lines = readFileLines(mat_file, c);
        // lines.resize(lines.size() - c);
        // std::ofstream outputFiles(mat_file, std::ios::trunc);
        // if (!outputFiles.is_open()) {
        //     std::cerr << "Error: Could not open the file for writing!" << std::endl;
        //     return 1;
        // }

        // // Write the remaining lines back to the file
        // for (const auto& remainingLine : lines) {
        //     outputFiles << remainingLine << std::endl;
        // }

        // outputFiles.close();
    }
    
}


int main(int argc, char* argv[])
{
    std::cout << "started" << std::endl;
    m1::ml_file(m1::mat_file,m1::num_runs);
    std::cout << "mmat_file: " << m1::mat_file << std::endl;
    std::cout << "mnum_runs: " << m1::num_runs << std::endl;
    main_start(argc,argv);
    std::cout << "end" << std::endl;
}