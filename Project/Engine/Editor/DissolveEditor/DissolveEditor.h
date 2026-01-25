#pragma once
#include "3d/Object3d/Object3d.h"
#include "DissolveData.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include <memory>
#include <string>

/// <summary>
/// ディゾルブエディタクラス
/// カテゴリーシステムを使用
/// </summary>
namespace KetaEngine {

class DissolveEditor : public BaseEffectEditor<DissolveData> {
public:
    DissolveEditor()  = default;
    ~DissolveEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectEditorからのオーバーライド
    void Init(const std::string& typeName) override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<DissolveData> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;
    std::string GetFolderName() const override;

private:
    //*---------------------------- private Variant ----------------------------*//

    std::unique_ptr<Object3d> preViewObj_;
    const std::string folderName_ = "DissolveEditor/";

    // プレビュー設定
    bool showPreview_      = true;
    float manualThreshold_ = 1.0f;
    bool manualEnable_     = false;

public:
    //*----------------------------- getter Methods -----------------------------*//
};

}; // KetaEngine