#pragma once
#include "utility/ParameterEditor/GlobalParameter.h"

class Player;
class FireInjectors;
class GameBackGroundObject;
class GameCamera;
class HowToOperate;

class GameIntro {
public:
    enum class IntroStep {
        START,
        WAIT,
        OBJSPAWN,
        PLAYERSPAWN,
        FINISH,
    };

public:
    GameIntro()  = default;
    ~GameIntro() = default;

    void Init();
    void Update();

    // stepMove
    void Start();
    void Wait();
    void ObjSpawn();
    void PlayerSpawn();
    void Finish();

    // Editor
    void AdjustParam();
    void BindParams();

private:
    static void (GameIntro::* spFuncTable_[])();

private:
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "GameIntro";

    // param
    float waitTime_;
    float objSpawnTime_;

    FireInjectors* pFireInjectors_ = nullptr;
    GameCamera* pGameCamera_       = nullptr;
    Player* pPlayer_               = nullptr;
    HowToOperate* pHowToOperate_   = nullptr;

    GameBackGroundObject* pGameBackGroundObject_ = nullptr;

    float currentWaitTime_;
    float currentObjSpawnTime_;
    bool isFinish_  = false;
    IntroStep step_ = IntroStep::START;

public:
    void SetHowToOperate(HowToOperate* howToOperate) { pHowToOperate_ = howToOperate; }
    void SetGameCamera(GameCamera* gameCamera) { pGameCamera_ = gameCamera; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
    void SetFireInjectors(FireInjectors* fireInjectors) { pFireInjectors_ = fireInjectors; }
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject) { pGameBackGroundObject_ = gameBackGroundObject; }
};
