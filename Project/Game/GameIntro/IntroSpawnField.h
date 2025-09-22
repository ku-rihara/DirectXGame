#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include "MovieLine.h"
#include <memory>

class IntroSpawnField : public BaseGameIntro {
public:
    enum class IntroStep {
        START,
        WAIT,
        OBJSPAWN,
        PLAYERSPAWN,
        PURPOSWAIT,
        PURPOSEAPPEAR,
        FINISH,
    };

public:
    IntroSpawnField()           = default;
    ~IntroSpawnField() override = default;

    // Init,Update,Draw
    void Init(const std::string&name) override;
    void Update() override;
    void Draw() override;
    
    // Param Edit
    void AdjustParam() override;

private:
    // Step functions
    void Start();
    void Wait();
    void ObjSpawn();
    void PlayerSpawn();
    void PurposeWait();
    void PurposeAppear();
    void Finish();

    // Utility functions
    bool ProcessStep(float limitTime, IntroStep nextStep, bool enableEnemySpawn = false);
    void BindParams();

private:
    static void (IntroSpawnField::* spFuncTable_[])();

private:
    // Parameters
    float waitTime_        = 2.0f;
    float objSpawnTime_    = 3.0f;
    float playerSpawnTime_ = 2.5f;
    float purposeWaitTime_ = 4.0f;

    // Objects
    std::unique_ptr<Sprite> sprite_       = nullptr;
    std::unique_ptr<MovieLine> movieLine_ = nullptr;

    // State
    IntroStep step_ = IntroStep::START;
};