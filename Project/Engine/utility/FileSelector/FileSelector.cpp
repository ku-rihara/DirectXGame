#include "FileSelector.h"
#include "Function/GetFile.h"
#include <algorithm>
#include <imgui.h>
#include <filesystem>


void FileSelector::SelectFile(
    const char* label,
    const std::string& directoryPath,
    std::string& currentSelection,
    const std::string& excludeName,
    bool addNoneOption) {

    // キャッシュの更新が必要か、ディレクトリが変わった場合
    if (needsRefresh_ || cachedDirectory_ != directoryPath || cachedFiles_.empty()) {
        cachedFiles_     = GetFileNamesForDirectory(directoryPath);
        cachedDirectory_ = directoryPath;

        // 除外ファイルがある場合は削除
        if (!excludeName.empty()) {
            cachedFiles_.erase(
                std::remove_if(cachedFiles_.begin(), cachedFiles_.end(),
                    [&excludeName](const std::string& name) {
                        return name == excludeName;
                    }),
                cachedFiles_.end());
        }

        // "None"オプションを先頭に追加
        if (addNoneOption) {
            cachedFiles_.insert(cachedFiles_.begin(), "None");
        }

        needsRefresh_ = false;
    }

    // 現在選択中のインデックスを取得
    int currentIndex = 0;
    for (int i = 0; i < static_cast<int>(cachedFiles_.size()); i++) {
        if (cachedFiles_[i] == currentSelection) {
            currentIndex = i;
            break;
        }
    }

    // コンボアイテムの準備
    std::vector<const char*> comboItems;
    for (const auto& name : cachedFiles_) {
        comboItems.push_back(name.c_str());
    }

    // コンボボックスで選択
    bool changed = false;
    if (ImGui::Combo(label, &currentIndex, comboItems.data(), static_cast<int>(comboItems.size()))) {
        currentSelection = cachedFiles_[currentIndex];
        changed          = true;
    }

    // リフレッシュボタン
    ImGui::SameLine();
    if (ImGui::SmallButton(("Refresh##" + std::string(label)).c_str())) {
        needsRefresh_ = true;
    }

 
}

