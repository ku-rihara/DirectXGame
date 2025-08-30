#pragma once
#include "2d/Sprite.h"
#include"MovieLine.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <memory>

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
        PURPOSWAIT,
        PURPOSEAPPEAR,
        FINISH,
    };

public:
    GameIntro()  = default;
    ~GameIntro() = default;

    void Init();
    void Update();
    void SpriteDraw();

    bool ProcessStep(float limitTime, IntroStep nextStep, bool enableEnemySpawn = false);

    // stepMove
    void Start();
    void Wait();
    void ObjSpawn();
    void PlayerSpawn();
    void PurposeWait();
    void PurposeAppear();
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
    float playerSpawnTime_;
    float purposeWaitTime_;

    FireInjectors* pFireInjectors_ = nullptr;
    GameCamera* pGameCamera_       = nullptr;
    Player* pPlayer_               = nullptr;
    HowToOperate* pHowToOperate_   = nullptr;

    std::unique_ptr<Sprite> sprite_       = nullptr;
    std::unique_ptr<MovieLine> movieLine_ = nullptr;

    GameBackGroundObject* pGameBackGroundObject_ = nullptr;

    // time
    float currentTime_;

    bool isFinish_         = false;
    bool isAbleEnemySpawn_ = false;
    IntroStep step_        = IntroStep::START;

public:
    bool GetIsAbleEnemySpawn() const { return isAbleEnemySpawn_; }
    bool GetIsFinish() const { return isFinish_; }

    void SetHowToOperate(HowToOperate* howToOperate) { pHowToOperate_ = howToOperate; }
    void SetGameCamera(GameCamera* gameCamera) { pGameCamera_ = gameCamera; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
    void SetFireInjectors(FireInjectors* fireInjectors) { pFireInjectors_ = fireInjectors; }
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject) { pGameBackGroundObject_ = gameBackGroundObject; }
};
