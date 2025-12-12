#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <memory>
#include <string>

class Player;
class FireInjectors;
class GameBackGroundObject;
class GameCamera;
class HowToOperate;

/// <summary>
/// ゲームイントロ基底クラス
/// </summary>
class BaseGameIntro {
public:
    BaseGameIntro()          = default;
    virtual ~BaseGameIntro() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="name">イントロ名</param>
    virtual void Init(const std::string& name);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="playSpeed">再生速度</param>
    virtual void Update(float playSpeed);

    // Param Edit
    virtual void RegisterParams();            //< パラメータバインド
    virtual void AdjustParam();           //< パラメータ調整
    virtual void AdjustUniqueParam() = 0; //< 固有パラメータ調整

protected:
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;
    FireInjectors* pFireInjectors_               = nullptr;
    GameCamera* pGameCamera_                     = nullptr;
    Player* pPlayer_                             = nullptr;
    HowToOperate* pHowToOperate_                 = nullptr;
    GameBackGroundObject* pGameBackGroundObject_ = nullptr;
    bool isFinish_                               = false;
    bool isAbleEnemySpawn_                       = false;
    float currentTime_                           = 0.0f;
    float playSpeed_;

public:
    // getter
    const bool& GetIsFinish() const { return isFinish_; }
    const bool& GetIsAbleEnemySpawn() const { return isAbleEnemySpawn_; }

    // setter
    void SetHowToOperate(HowToOperate* howToOperate) { pHowToOperate_ = howToOperate; }
    void SetGameCamera(GameCamera* gameCamera) { pGameCamera_ = gameCamera; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
    void SetFireInjectors(FireInjectors* fireInjectors) { pFireInjectors_ = fireInjectors; }
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject) { pGameBackGroundObject_ = gameBackGroundObject; }
};