#include"GetFile.h"
#include<filesystem>
///------------------------------------------------------------------------------------------------
/// パーティクルファイル取得
///------------------------------------------------------------------------------------------------
std::vector<std::string> GetFileNamesForDyrectry(const std::string& directory) {
    std::vector<std::string> fileNames;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {  
            fileNames.push_back(entry.path().stem().string());  // ← ここで拡張子を削除
        }
    }
    return fileNames;
}
