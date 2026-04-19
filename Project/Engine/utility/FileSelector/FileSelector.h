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

    /// <summary>
    /// フルパスを直接渡してファイルを選択するコンボボックスを表示
    /// ステム変換・パス再構築を内部で行うため呼び出し側はパスを渡すだけでよい
    /// </summary>
    /// <param name="label">表示ラベル</param>
    /// <param name="directoryPath">検索するディレクトリパス</param>
    /// <param name="currentPath">現在のフルパス（変更時に更新される）</param>
    /// <param name="extension">拡張子（例: ".dds"）</param>
    /// <param name="addNoneOption">先頭に"None"を追加するか</param>
    void SelectFilePath(
        const char* label,
        const std::string& directoryPath,
        std::string& currentPath,
        const std::string& extension = ".dds",
        bool addNoneOption           = false);

    /// <summary>
    /// ファイルパスからステム名（拡張子なし・ディレクトリなし）を返す
    /// パスが空の場合は "None" を返す
    /// </summary>
    static std::string PathToStem(const std::string& path);

private:
    std::vector<std::string> cachedFiles_;
    std::string cachedDirectory_;
    bool needsRefresh_ = false;

public:
    const std::vector<std::string>& GetCachedFiles() const { return cachedFiles_; }
    const std::string& GetCachedDirectory() const { return cachedDirectory_; }
    bool GetNeedsRefresh() const { return needsRefresh_; }
};

}; // KetaEngine
