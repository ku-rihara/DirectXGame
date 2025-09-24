#pragma once
#include "BaseGameIntro.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <array>

class GameIntroManager {
public:

    enum IntroStep {
        SpawnField,
        AppearPurpose,
        COUNT,
    };

public:
    GameIntroManager()  = default;
    ~GameIntroManager() = default;

    void Init();
    void Update();
    void UIDraw();

    // param Edit
    void BindParam();
    void AdjustParam();

  
    // State
    bool IsAllIntroFinished() const;

private:
    void ProcessInput();
    void UpdateCurrentIntro(const float& speed);
    void MoveToNextIntro();

private:
    GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_      = "GameIntroManager";

    std::array<std::unique_ptr<BaseGameIntro>, static_cast<size_t>(IntroStep::COUNT)> introSequences_;
    int32_t currentIndex_ = 0;
    bool isInitialized_   = false;

    // speed Param
    float currentPlaySpeedRate_ = 1.0f; //<現在再生スピード
    float fastSpeedRate_        = 3.0f; //<早送りスピード

    // Dependencies
    HowToOperate* pHowToOperate_                 = nullptr;
    GameCamera* pGameCamera_                     = nullptr;
    Player* pPlayer_                             = nullptr;
    FireInjectors* pFireInjectors_               = nullptr;
    GameBackGroundObject* pGameBackGroundObject_ = nullptr;

public:
    //-------------------------------------------------------------------------------------
    // getter method
    //-------------------------------------------------------------------------------------
    const int32_t& GetCurrentIntroIndex() const { return currentIndex_; }
    int32_t GetTotalIntroCount() const { return static_cast<int32_t>(introSequences_.size()); }
    const float& GetCurrentPlaySpeedRate() const { return currentPlaySpeedRate_; }
    const bool& GetIsFinishStep(const IntroStep& step);

    //-------------------------------------------------------------------------------------
    // setter method
    //-------------------------------------------------------------------------------------
    void SetHowToOperate(HowToOperate* howToOperate);
    void SetGameCamera(GameCamera* gameCamera);
    void SetPlayer(Player* player);
    void SetFireInjectors(FireInjectors* fireInjectors);
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject);
    void SetSpeedMultiplier(const float& multiplier) { currentPlaySpeedRate_ = multiplier; }
    void ClassisSet();
};