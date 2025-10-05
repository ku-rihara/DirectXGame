#pragma once

// 前方宣言
class Player;
class Combo;
class StadiumLightEffect;

class ComboScene {
public:
    enum class State {
        CHECK,
        LEVELUP,
        RESET,
    };

public:
    ComboScene()  = default;
    ~ComboScene() = default;

    // 初期化、更新、描画
    void Init();
    void Update();

    void CheckIsLevelUp();
    void LevelUp();
    void LevelReset();

private:
    // メンバ関数のポインタ配列
    static void (ComboScene::* spFuncTable_[])();

private:
    Player* pPlayer_ = nullptr;
    Combo* pCombo_   = nullptr;
    StadiumLightEffect* stadiumLightEffect_ = nullptr;

    State state_ = State::CHECK;

public: // accessor
    void SetPlayer(Player* player);
    void SetCombo(Combo* combo);
    void SetStadiumLightEffect(StadiumLightEffect* stadiumLightEffect);
};