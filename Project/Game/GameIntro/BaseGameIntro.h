#pragma once
#include "utility/ParameterEditor/GlobalParameter.h"
#include <memory>
#include <string>

class Player;
class FireInjectors;
class GameBackGroundObject;
class GameCamera;
class HowToOperate;

class BaseGameIntro {
public:
    BaseGameIntro()          = default;
    virtual ~BaseGameIntro() = default;

    // Init,Update,Draw
    virtual void Init(const std::string& name);
    virtual void Update(const float& playSpeed);
    virtual void Draw() = 0;

    // Param Edit
    virtual void BindParams();
    virtual void AdjustParam();
    virtual void AdjustUniqueParam() = 0;

protected:
    GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;

    // Dependencies
    FireInjectors* pFireInjectors_               = nullptr;
    GameCamera* pGameCamera_                     = nullptr;
    Player* pPlayer_                             = nullptr;
    HowToOperate* pHowToOperate_                 = nullptr;
    GameBackGroundObject* pGameBackGroundObject_ = nullptr;

    // Common state
    bool isFinish_         = false;
    bool isAbleEnemySpawn_ = false;
    float currentTime_     = 0.0f;
    float playSpeed_;

public:
    // Getters
    bool GetIsFinish() const { return isFinish_; }
    bool GetIsAbleEnemySpawn() const { return isAbleEnemySpawn_; }

    // Setters for dependencies
    void SetHowToOperate(HowToOperate* howToOperate) { pHowToOperate_ = howToOperate; }
    void SetGameCamera(GameCamera* gameCamera) { pGameCamera_ = gameCamera; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
    void SetFireInjectors(FireInjectors* fireInjectors) { pFireInjectors_ = fireInjectors; }
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject) { pGameBackGroundObject_ = gameBackGroundObject; }
};