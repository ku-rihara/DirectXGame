#pragma once
#include "2d/Sprite.h"
#include "BaseGameIntro.h"
#include "MovieLine.h"
#include <memory>

class IntroSpawnField : public BaseGameIntro {
public:
    enum class Step {
        START,
        WAIT,
        OBJSPAWN,
        PLAYERSPAWN,
        PURPOSWAIT,
        FINISH,
    };

public:
    IntroSpawnField()           = default;
    ~IntroSpawnField() override = default;

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
    void Start();
    void Wait();
    void ObjSpawn();
    void PlayerSpawn();
    void FinishWait();
    void Finish();

    // Step更新
    bool ProcessStep(const float& limitTime, const Step& nextStep, const bool& enableEnemySpawn = false);

private:
    static void (IntroSpawnField::* spFuncTable_[])();

private:
    // Parameters
    float waitTime_;
    float objSpawnTime_;
    float playerSpawnTime_;
    float finishWaitTime_;

   
    // State
    Step step_ = Step::START;

public:
    const Step& GetCurrentStep() const { return step_; }
};