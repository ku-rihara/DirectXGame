#pragma once

#include "BaseEnemy.h"
#include "3d/ViewProjection.h"

/// std
#include <vector>
#include <list>
#include <memory>
#include <string>
#include<map>
#include <json.hpp>

class Player;
class EnemyManager {
private:
    using json = nlohmann::json;
private:

    struct SpownEnemy {
        std::string enemyType;    // 敵の種類
        Vector3 position;         // 敵の座標
    };

    struct EnemyGroup {
        std::vector<SpownEnemy> spownEnemies; // 敵グループのリスト
        float spownTime;                 // グループ生成間隔
        bool isSpowned;
    };


    struct Wave {
        float startTime;                   // Waveの開始時間（フェーズ内の相対時間）
        std::vector<EnemyGroup> groups;    // Wave内で発生する敵の情報
    };

    struct Phase {
        std::vector<Wave> waves; // フェーズ内のWaveリスト
    };

    ///========================================================
    /// Private variants
    ///========================================================

    // ohter class
    Player* pPlayer_;
  
  
    bool isEditorMode_;             // エディタモード中かどうか
    std::map<int, Phase> phases_;  // フェーズ番号をキーとしたフェーズマップ
    int currentPhase_;             // 現在のフェーズ
    float currentTime_;            // 現在のフェーズ内の経過時間
    int currentWave_;
    ///* 敵リスト
    std::list<std::unique_ptr<BaseEnemy>> enemies_;

    ///* 敵の種類リスト
    std::vector<std::string> enemyTypes_ = { "NormalEnemy","StrongEnemy"};

    //* 一時的な敵生成用データ
    std::string selectedEnemyType_;
    Vector3 spownPosition_;
    uint32_t spownNum_;

  
    const std::string directrypath_ = "./resources/EnemyParamater/";// path
    const std::string filename_ = "PoPData.json";// name

public:

    ///========================================================
    /// public method
    ///========================================================

    // コンストラクタ
    EnemyManager();

    // 初期化
    void Init();

    // 敵の生成
    void SpawnEnemy(const std::string& enemyType, const Vector3& position);

    // 更新処理
    void Update();
    void HpBarUpdate(const ViewProjection&viewProjection);
    void SpawnUpdate();
    void CheckWaveCompletion();
    // 描画処理
    void Draw(const ViewProjection& viewProjection);

    // スプライト描画処理
    void SpriteDraw(const ViewProjection& viewProjection);

    ///========================================================
   /// editor method
   ///========================================================

   
    void ImGuiUpdate();/// ImGuiによるエディタ
    void SaveAndLoad();/// セーブとロード

    ///* セーブ
    void SaveEnemyPoPData();

    ///* ロード
    void LoadEnemyPoPData();
    void LoadPhase(Phase& phase, const json& phaseData);
    void LoadSpawn(EnemyGroup& spawn, const json& spawnData);

    ///* EditorModeセット
    void SetEditorMode(bool isEditorMode);

    ///========================================================
    /// setter method
    ///========================================================
    void SetPlayer(Player* plyaer);

    // フェーズの切り替え
    void SetPhase(int phase);

    // 現在の敵リスト取得
    const std::list<std::unique_ptr<BaseEnemy>>& GetEnemies() const {
        return enemies_;}
};

