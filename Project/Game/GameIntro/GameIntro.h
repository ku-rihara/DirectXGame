#pragma once

class Player;
class FireInjectors;
class GameBackGroundObject;

class GameIntro {
public:
    enum class IntroStep {
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
    void Wait();
    void ObjSpawn();
    void PlayerSpawn();
    void Finish();

private:
    static void (GameIntro::* spFuncTable_[])();

private:

    Player* pPlayer_                             = nullptr;
    FireInjectors* pFireInjectors_               = nullptr;
    GameBackGroundObject* pGameBackGroundObject_ = nullptr;

    bool isFinish_  = false;
    IntroStep step_ = IntroStep::WAIT;

public:
    void SetPlayer(Player* player) { pPlayer_ = player; }
    void SetFireInjectors(FireInjectors* fireInjectors) { pFireInjectors_ = fireInjectors; }
    void SetGameBackGroundObject(GameBackGroundObject* gameBackGroundObject) { pGameBackGroundObject_ = gameBackGroundObject; }
};
