#pragma once
#include "3d/Object3d.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "ShakeData.h"
#include <memory>
#include <string>

/// <summary>
/// シェイクエディター
/// </summary>
namespace KetaEngine {

class ShakeEditor : public BaseEffectEditor<ShakeData> {
public:
    ShakeEditor()  = default;
    ~ShakeEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // 初期化、更新
    void Init(const std::string& editorName, const bool& isUseCategory = false) override;
    void Update(float speedRate = 1.0f) override;
    void EditorUpdate() override;

private:
    //*---------------------------- private Methods ----------------------------*//

    // 純粋仮想関数の実装
    std::unique_ptr<ShakeData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderPath() const override;
    void PlaySelectedAnimation() override;

private:
    //*---------------------------- private Variant ----------------------------*//

    std::unique_ptr<Object3d> preViewObj_;
    const std::string folderName_ = "ShakeEditor/";
    Vector3 basePos_              = Vector3::ZeroVector();
    bool isPreViewDraw_           = false;

public:
    //*----------------------------- getter Methods -----------------------------*//

    int GetShakeCount() const { return static_cast<int>(effects_.size()); }
};

}; // KetaEngine
