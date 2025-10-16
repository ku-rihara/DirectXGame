#pragma once
#include "BaseGameIntro.h"
#include "MovieLine.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

class GameIntroManager {
public:
    enum IntroStep {
        SpawnField,
        AppearPurpose,
        COUNT,
    };

    enum class MovieLineState {
        NONE,
        APPEAR,
        EXIT
    };

public:
    GameIntroManager()  = default;
    ~GameIntroManager() = default;

    // 初期化、更新
    void Init();
    void Update();
    
    // movieLine
    void MovieLineUpdate();
    void CheckMovieLineStateTransition();

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

    // MovieLine管理
    std::unique_ptr<MovieLine> movieLine_ = nullptr;
    MovieLineState movieLineState_        = MovieLineState::NONE;

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
    const int32_t& GetTotalIntroCount() const { return static_cast<int32_t>(introSequences_.size()); }
    const float& GetCurrentPlaySpeedRate() const { return currentPlaySpeedRate_; }
    const bool& GetIsFinishStep(const IntroStep& step);
    MovieLine* GetMovieLine() const { return movieLine_.get(); }

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