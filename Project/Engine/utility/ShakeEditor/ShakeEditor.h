#pragma once
#include "3d/Object3d.h"
#include "ShakeData.h"
#include <memory>
#include <string>
#include <vector>

class ShakeEditor {
public:
    ShakeEditor()  = default;
    ~ShakeEditor() = default;

    /// 初期化
    void Init();
    void Update(const float& deltaTime);
    void EditorUpdate();

    /// シェイク管理
    void AddShake(const std::string& shakeName);
    void RemoveShake(const int& index);
    void AllLoadFile();
    void AllSaveFile();

    /// 取得
    ShakeData* GetSelectedShake();
    ShakeData* GetShakeByName(const std::string& name);
    const int& GetShakeCount() const { return static_cast<int>(shakes_.size()); }

private:
    std::vector<std::unique_ptr<ShakeData>> shakes_;
    std::unique_ptr<Object3d> preViewObj_;

    Vector3 basePos_      = Vector3::ZeroVector();
    int selectedIndex_ = -1;

    bool isPreViewDraw_;

    // 入力用バッファ
    char nameBuffer_[128] = "";
};