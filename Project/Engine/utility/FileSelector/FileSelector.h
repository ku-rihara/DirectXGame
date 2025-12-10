#pragma once
#include <string>
#include <vector>

namespace KetaEngine {

class FileSelector {
public:
    FileSelector()  = default;
    ~FileSelector() = default;

    /// <summary>
    /// ImGuiでファイルを選択するコンボボックスを表示
    /// </summary>
    /// <param name="label">表示ラベル</param>
    /// <param name="directoryPath">検索するディレクトリパス</param>
    /// <param name="currentSelection">現在選択されている値（</param>
    /// <param name="excludeName">除外するファイル名</param>
    /// <param name="addNoneOption">先頭に"None"を追加するか</param>
    /// <returns>選択が変更されたらtrue</returns>
    void SelectFile(
        const char* label,
        const std::string& directoryPath,
        std::string& currentSelection,
        const std::string& excludeName = "",
        bool addNoneOption             = false);

private:
    std::vector<std::string> cachedFiles_;
    std::string cachedDirectory_;
    bool needsRefresh_;

public:
    const std::vector<std::string>& GetCachedFiles() const { return cachedFiles_; }
    const std::string& GetCachedDirectory() const { return cachedDirectory_; }
    const bool& GetNeedsRefresh() const { return needsRefresh_; }
};

}; // KetaEngine
