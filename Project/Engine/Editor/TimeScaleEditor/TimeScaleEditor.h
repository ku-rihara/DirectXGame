#pragma once
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include "TimeScaleData.h"
#include <memory>
#include <string>

/// <summary>
/// タイムスケールエディター
/// </summary>
namespace KetaEngine {

class TimeScaleEditor : public BaseEffectEditor<TimeScaleData> {
public:
    TimeScaleEditor()  = default;
    ~TimeScaleEditor() = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectEditorからのオーバーライド
    void Init(const std::string& typeName) override;
    void Update(float deltaTime) override;
   
protected:
    //*---------------------------- protected Methods ----------------------------*//

    std::unique_ptr<TimeScaleData> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;
    std::string GetFolderName() const override;

public:
    //*----------------------------- getter Methods -----------------------------*//
    
};

}; // KetaEngine