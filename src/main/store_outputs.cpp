#include <iostream>
#include <fstream>
#include <sys/stat.h>   // For mkdir on Unix/Linux
    // For mkdir on Windows
#include <string>
#include <cstdio>       // For std::remove

#include <sys/stat.h> // POSIX (for macOS/Linux)
#include <sys/types.h>
#include <vector>  // for std::vector
#include <string>  // for std::string



#ifdef _WIN32
    #include <direct.h> // Windows specific header
    #define MKDIR(dir) _mkdir(dir) // _mkdir for Windows
#else
    #define MKDIR(dir) mkdir(dir, 0777) // mkdir for macOS/Linux
#endif

void copyFile(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dest(destination, std::ios::binary);

    if (!src.is_open()) {
        std::cerr << "Error: Could not open source file " << source << std::endl;
        return;
    }
    if (!dest.is_open()) {
        std::cerr << "Error: Could not open destination file " << destination << std::endl;
        return;
    }

    dest << src.rdbuf(); // Copy the file content
    src.close();
    dest.close();
}


void copyFilesToFolder(int c) {
    // Create the target folder ML_outputs/c
    std::string folderPath = "ML_outputs/" + std::to_string(c);

#ifdef _WIN32
    _mkdir("ML_outputs"); // Create ML_outputs directory on Windows
    _mkdir(folderPath.c_str()); // Create ML_outputs/c directory on Windows
#else
    mkdir("ML_outputs", 0777); // Create ML_outputs directory on Linux/Unix
    mkdir(folderPath.c_str(), 0777); // Create ML_outputs/c directory on Linux/Unix
#endif

    // List of files to copy
    std::vector<std::string> files = {"input", "ML", "log", "output"};

    // Copy each file into the created folder
    for (const auto& file : files) {
        std::string sourceFile = file;
        std::string destinationFile = folderPath + "/" + file;

        std::cout << "Copying " << sourceFile << " to " << destinationFile << std::endl;
        copyFile(sourceFile, destinationFile);

    }

    std::cout << "Files copied to " << folderPath << std::endl;
}
