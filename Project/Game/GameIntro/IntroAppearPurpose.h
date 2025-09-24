#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
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

    // Step更新
    bool ProcessStep(const float& limitTime, const Step& nextStep, const bool& enableEnemySpawn = false);

private:
    static void (IntroAppearPurpose::* spFuncTable_[])();

private:
    // Parameters
    float appearWaitTime_;

    // Objects
    std::unique_ptr<Sprite> sprite_       = nullptr;
   
    // State
    Step step_ = Step::APPEARWAIT;
};