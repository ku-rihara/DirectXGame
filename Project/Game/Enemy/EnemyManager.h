#pragma once

#include "BaseEnemy.h"
#include "3d/ViewProjection.h"

/// std
#include <vector>
#include <list>
#include <memory>
#include<utility>
#include <string>
#include<map>
#include <json.hpp>

class EnemyManager {
private:
    using json = nlohmann::json;
private:

    struct EnemyGroup {
        std::string enemyType;    // 敵の種類
        Vector3 position;         // 敵の座標
 
    };

    struct EnemySpawn {
        std::vector<EnemyGroup> enemyGroups; // 敵グループのリスト
        float spownTime;                 // グループ生成間隔
        
    };


    struct Wave {
        float startTime;                   // Waveの開始時間（フェーズ内の相対時間）
        std::vector<EnemySpawn> spawns;    // Wave内で発生する敵の情報
    };

    struct Phase {
        std::vector<Wave> waves; // フェーズ内のWaveリスト
    };

    ///========================================================
    /// Private variants
    ///========================================================

  
    bool isEditorMode_;             // エディタモード中かどうか
    std::map<int, Phase> phases_;  // フェーズ番号をキーとしたフェーズマップ
    int currentPhase_;             // 現在のフェーズ
    float currentTime_;            // 現在のフェーズ内の経過時間

    ///* 敵リスト
    std::list<std::unique_ptr<BaseEnemy>> enemies_;

    ///* 敵の種類リスト
    std::vector<std::string> enemyTypes_ = { "NormalEnemy" };

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
    void SpawnEnemy(const std::vector<Vector3>& spawnPositions);

    // 更新処理
    void Update();

    // 描画処理
    void Draw(const ViewProjection& viewProjection);

    // スプライト描画処理
    void SpriteDraw(const ViewProjection& viewProjection);

    ///========================================================
   /// editor method
   ///========================================================

    // ImGuiによるエディタ
    void ImGuiUpdate();
    void SaveAndLoad();
    void SetEditorMode(bool isEditorMode);

    ///========================================================
    /// setter method
    ///========================================================
    
    // フェーズの切り替え
    void SetPhase(int phase);

    // 現在の敵リスト取得
    const std::list<std::unique_ptr<BaseEnemy>>& GetEnemies() const {
        return enemies_;}
};

