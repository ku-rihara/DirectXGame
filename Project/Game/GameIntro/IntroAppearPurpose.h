#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include "Easing/Easing.h"
#include "MovieLine.h"
#include <array>
#include <cstdint>
#include <memory>

class IntroAppearPurpose : public BaseGameIntro {
public:
    enum class Step {
        SIDEAPPEARWAIT,
        SIDEAPPEAR,
        CENTERAPPEAR,
        FINISHWAIT,
        FINISH,
    };

    enum SpritePos {
        LEFT,
        RIGHT,
        CENTER,
        COUNT,
    };

    struct SpriteVariable {
        std::array<float, 2> sideAppearPosX;
        std::array<float, 2> sideBackPosX;
        float scaleY;
        Vector2 centerScale;
        std::array<std::unique_ptr<Easing<float>>, 2> sideAppearEase;
        std::array<std::unique_ptr<Easing<float>>, 2> sideBackEase;
        std::unique_ptr<Easing<Vector2>> centerAppearEase;
        std::unique_ptr<Easing<float>> scaleEaseY;
        bool isBackSideUI = false;
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
    void SideAppearWait();
    void SideAppear();
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
    float disAppearWaitTime_;
    float finishWaitTime_;

    // Objects
    std::unique_ptr<Sprite> backLineSprite_;
    std::array<std::unique_ptr<Sprite>, static_cast<size_t>(SpritePos::COUNT)> sprites_;
    SpriteVariable spriteVariable_;

    // State
    Step step_ = Step::SIDEAPPEARWAIT;
};