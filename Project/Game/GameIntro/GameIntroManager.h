#pragma once
#include "BaseGameIntro.h"
#include "MovieLine.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

/// <summary>
/// ゲームイントロ管理クラス
/// </summary>
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

    void MovieLineUpdate();               //< ムービーライン更新
    void CheckMovieLineStateTransition(); //< ムービーライン状態遷移チェック

    void BindParam();     //< パラメータバインド
    void AdjustParam();   //< パラメータ調整

    bool IsAllIntroFinished() const; //< 全イントロ終了判定

    /// <summary>
    /// 指定ステップの終了判定
    /// </summary>
    /// <param name="step">イントロステップ</param>
    /// <returns>終了していればtrue</returns>
    const bool& GetIsFinishStep(const IntroStep& step);

private:
    
    /// <summary>
    /// 現在のイントロを更新
    /// </summary>
    /// <param name="speed">再生速度</param>
    void UpdateCurrentIntro(float speed);

    void ProcessInput(); //< 入力処理
    void MoveToNextIntro(); //< 次のイントロへ移行

private:
    GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_      = "GameIntroManager";
    std::array<std::unique_ptr<BaseGameIntro>, static_cast<size_t>(IntroStep::COUNT)> introSequences_;
    int32_t currentIndex_                        = 0;
    bool isInitialized_                          = false;
    std::unique_ptr<MovieLine> movieLine_        = nullptr;
    MovieLineState movieLineState_               = MovieLineState::NONE;
    float currentPlaySpeedRate_                  = 1.0f;
    float fastSpeedRate_                         = 3.0f;
    HowToOperate* pHowToOperate_                 = nullptr;
    GameCamera* pGameCamera_                     = nullptr;
    Player* pPlayer_                             = nullptr;
    FireInjectors* pFireInjectors_               = nullptr;
    GameBackGroundObject* pGameBackGroundObject_ = nullptr;

public:
    // getter
    const int32_t& GetCurrentIntroIndex() const { return currentIndex_; }
    const int32_t& GetTotalIntroCount() const { return static_cast<int32_t>(introSequences_.size()); }
    float GetCurrentPlaySpeedRate() const { return currentPlaySpeedRate_; }
    MovieLine* GetMovieLine() const { return movieLine_.get(); }

    // setter
    void SetHowToOperate(HowToOperate* howToOperate);
    void SetGameCamera(GameCamera* gameCamera);
    void SetPlayer(Player* player);
    void SetFireInjectors(FireInjectors* fireInjectors);
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject);
    void SetSpeedMultiplier(float multiplier) { currentPlaySpeedRate_ = multiplier; }
    void ClassisSet();
};