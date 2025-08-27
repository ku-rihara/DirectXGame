#pragma once
#include"utility/TimeScaleEditor/TimeScaleController.h"
#include"utility/PostEffectEditor/PostEffectController.h"
#include <memory>

class AttackEffect {
public:
    AttackEffect()=default;
    ~AttackEffect() = default;

    void Init();
    void Update();
    void PlayHitStop(const std::string& timeScaleName);
    void PlayPostEffect(const std::string& effectName);

    void EditorUpdate();

private:
    std::unique_ptr<TimeScaleController> timeScaleController_;
    std::unique_ptr<PostEffectController>postEffectController_;

};