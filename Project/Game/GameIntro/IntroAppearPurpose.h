#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include "Easing/Easing.h"
#include "MovieLine.h"
#include <memory>

class IntroAppearPurpose : public BaseGameIntro {
public:
    enum class Step {
        APPEARWAIT,
        APPEAR,
        DISAPPEARWAIT,
        DISAPPEAR,
        FINISHWAIT,
        FINISH,
    };

    struct SpriteVariable {
        float currentRotateX;
        Vector2 currentScale;
        std::unique_ptr<Easing<float>> rotateEasing;
        std::unique_ptr<Easing<Vector2>> scaleEasing;
    };

public:
    IntroAppearPurpose()           = default;
    ~IntroAppearPurpose() override = default;

    // Init,Update,Draw
    void Init(const std::string& name) override;
    void Update(const float& playSpeed) override;
    void Draw() override;

    // Param Edit
    void AdjustParam() override;
    void BindParams() override;
    void AdjustUniqueParam() override;

private:
    // Step function
    void APPEARWAIT();
    void APPEAR();
    void DISAPPEARWAIT();
    void DISAPPEAR();
    void FinishWait();
    void Finish();

    void EasingInit();

    // Step更新
    bool ProcessStep(const float& limitTime, const Step& nextStep, const bool& enableEnemySpawn = false);

private:
    static void (IntroAppearPurpose::* spFuncTable_[])();

private:
    // Parameters
    float appearWaitTime_;

    // Objects
    std::unique_ptr<Sprite> sprite_ = nullptr;
    SpriteVariable spriteVariable_;

    // State
    Step step_ = Step::APPEARWAIT;
};