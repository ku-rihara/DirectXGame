#pragma once
#include "3d/Object3d.h"
#include "DissolveData.h"
#include <memory>
#include <string>
#include <vector>

class DissolveEditor {
public:
    DissolveEditor()  = default;
    ~DissolveEditor() = default;

    /// 初期化
    void Init();
    void Update(float deltaTime);
    void EditorUpdate();

    /// ディゾルブ管理
    void AddDissolve(const std::string& dissolveName);
    void RemoveDissolve(int index);
    void AllLoadFile();
    void AllSaveFile();

    /// 取得
    DissolveData* GetSelectedDissolve();
    DissolveData* GetDissolveByName(const std::string& name);
    int GetDissolveCount() const { return static_cast<int>(dissolves_.size()); }

private:
    std::vector<std::unique_ptr<DissolveData>> dissolves_;
    std::unique_ptr<Object3d> preViewObj_;
    int selectedIndex_ = -1;

    // 入力用バッファ
    char nameBuffer_[128] = "";
};