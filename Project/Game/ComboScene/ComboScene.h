#pragma once
#include <cstdint>

// 前方宣言
class Player;
class Combo;
class ComboLevelObjHolder;
class AudienceController;
class DeathTimer;

/// <summary>
/// コンボシーンの制御クラス
/// </summary>
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

    // 初期化、更新
    void Init();
    void Update();

    void CheckIsLevelUp(); //< レベルアップチェック
    void LevelUp();        //< レベルアップ処理
    void LevelReset();     //< レベルリセット

private:
    // メンバ関数のポインタ配列
    static void (ComboScene::* spFuncTable_[])();

    int32_t GetComboLevelZeroStart();

private:
    Player* pPlayer_                          = nullptr;
    Combo* pCombo_                            = nullptr;
    ComboLevelObjHolder* comboLevelObjHolder_ = nullptr;
    AudienceController* audienceController_   = nullptr;
    DeathTimer* deathTimer_                   = nullptr;

    const int32_t levelOffset_ = 1;

    State state_ = State::CHECK;

public:
    // setter
    void SetPlayer(Player* player);
    void SetCombo(Combo* combo);
    void SetComboLevelObjHolder(ComboLevelObjHolder* comboLevelObjHolder);
    void SetAudienceController(AudienceController* audienceController);
    void SetDeathTimer(DeathTimer* deathTimer);
};