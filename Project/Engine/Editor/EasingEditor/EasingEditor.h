#pragma once
#include "EasingData.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "Vector2.h"
#include "Vector3.h"
#include <memory>
#include <string>

/// <summary>
/// イージングエディター（テンプレート版）
/// float / Vector2 / Vector3 を型引数で切り替える
/// </summary>
namespace KetaEngine {

template <typename T>
class EasingEditor : public BaseEffectEditor<EasingData<T>> {
public:
    EasingEditor()  = default;
    ~EasingEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectEditorからのオーバーライド
    void Init(const std::string& typeName) override;
    void Update(float speedRate = 1.0f) override;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<EasingData<T>> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;
    std::string GetFolderName() const override;
};

}; // KetaEngine
