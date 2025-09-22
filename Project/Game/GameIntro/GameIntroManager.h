#pragma once
#include "BaseGameIntro.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <memory>
#include <vector>

class GameIntroManager {
public:
    GameIntroManager()  = default;
    ~GameIntroManager() = default;

    void Init();
    void Update();
    void Draw();

    // param Edit
    void BindParam();
    void AdjustParam();

    // Add intro sequences
    void AddIntroSequence(std::unique_ptr<BaseGameIntro> intro);

    // State
    bool IsAllIntroFinished() const;

private:
    void ProcessInput();
    void UpdateCurrentIntro();
    void MoveToNextIntro();

private:
    GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_      = "GameIntroManager";

    std::vector<std::unique_ptr<BaseGameIntro>> introSequences_;
    int32_t currentIndex_ = 0;
    bool isInitialized_   = false;

    // speed Param
    float currentPlaySpeed_ = 1.0f; //<現在再生スピード
    float fastSpeed_        = 3.0f; //<早送りスピード

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
    const float& GetSpeedMultiplier() const { return currentPlaySpeed_; }

    //-------------------------------------------------------------------------------------
    // setter method
    //-------------------------------------------------------------------------------------
    void SetHowToOperate(HowToOperate* howToOperate);
    void SetGameCamera(GameCamera* gameCamera);
    void SetPlayer(Player* player);
    void SetFireInjectors(FireInjectors* fireInjectors);
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject);
    void SetSpeedMultiplier(const float& multiplier) { currentPlaySpeed_ = multiplier; }
};