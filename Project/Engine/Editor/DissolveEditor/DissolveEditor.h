#pragma once
#include "3d/Object3d/Object3d.h"
#include "DissolveData.h"
#include "Editor/BaseEffectEditor/BaseEffectEditor.h"
#include <memory>
#include <string>

/// <summary>
/// ディゾルブエディタクラス
/// </summary>
namespace KetaEngine {

class DissolveEditor : public BaseEffectEditor<DissolveData> {
public:
    DissolveEditor()  = default;
    ~DissolveEditor() = default;

    void Init(const std::string& name, bool isUseCategory = false)override;

protected:
    std::unique_ptr<DissolveData> CreateEffectData() override;
    void RenderSpecificUI() override;
    void PlaySelectedAnimation() override;
    std::string GetFolderPath() const override;

private:
    std::unique_ptr<Object3d> preViewObj_;
};

}; // KetaEngine