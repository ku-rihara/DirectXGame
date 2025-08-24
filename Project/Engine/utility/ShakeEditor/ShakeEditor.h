#pragma once
#include "ShakeData.h"
#include"3d/Object3d.h"
#include <memory>
#include <string>
#include <vector>

class ShakeEditor {
public:
    ShakeEditor()  = default;
    ~ShakeEditor() = default;

    /// 初期化
    void Init();
    void Update(float deltaTime);
    void EditorUpdate();

    /// シェイク管理
    void AddShake(const std::string& shakeName);
    void RemoveShake(int index);
    void AllLoadFile();
    void AllSaveFile();

    /// 取得
    ShakeData* GetSelectedShake();
    ShakeData* GetShakeByName(const std::string& name);
    int GetShakeCount() const { return static_cast<int>(shakes_.size()); }

private:
    std::vector<std::unique_ptr<ShakeData>> shakes_;
    std::unique_ptr<Object3d> preViewObj_;
    int selectedIndex_ = -1;

    // 入力用バッファ
    char nameBuffer_[128] = "";
};