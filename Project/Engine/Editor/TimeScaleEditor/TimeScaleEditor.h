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

    // 初期化、更新
    void Init(const std::string& editorName, bool isUseCategory = false) override;
    void Update(float deltaTime) override;
    void EditorUpdate() override;

private:
    //*---------------------------- private Methods ----------------------------*//

    // 純粋仮想関数の実装
    std::unique_ptr<TimeScaleData> CreateEffectData() override;
    void RenderSpecificUI() override;
    std::string GetFolderPath() const override;
    void PlaySelectedAnimation() override;

private:
    //*---------------------------- private Variant ----------------------------*//

    const std::string folderName_ = "TimeScale/";

public:
    //*----------------------------- getter Methods -----------------------------*//

    int GetTimeScaleCount() const { return static_cast<int>(effects_.size()); }
};

}; // KetaEngine