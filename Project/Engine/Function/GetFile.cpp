#include"GetFile.h"
#include<filesystem>

std::vector<std::string> GetFileNamesForDirectory(const std::string& directory) {
    std::vector<std::string> fileNames;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {  
            fileNames.push_back(entry.path().stem().string());
        }
    }
    return fileNames;
}
