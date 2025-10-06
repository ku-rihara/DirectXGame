#pragma once
#include <cstdint>

// 前方宣言
class Player;
class Combo;
class ComboLevelObjHolder;

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
    ComboLevelObjHolder* comboLevelObjHolder_ = nullptr;
    const int32_t levelOffset_                = 1;

    State state_ = State::CHECK;

public: // accessor
    void SetPlayer(Player* player);
    void SetCombo(Combo* combo);
    void SetComboLevelObjHolder(ComboLevelObjHolder* comboLevelObjHolder);
};