// EnemySpawner.h
#pragma once

#include "Enemy/BaseEnemy.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <json.hpp>

class EnemyManager;
class EnemySpawner {
private:
    using json = nlohmann::json;

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

private:
  
    // 敵の種類リスト
    std::vector<std::string> enemyTypes_ = { "NormalEnemy", "StrongEnemy" };

private:

    EnemyManager* pEnemyManager_ = nullptr;

    std::map<int, Phase> phases_;   // フェーズ番号をキーとしたフェーズマップ
    int   currentPhase_;            // 現在のフェーズ
    float currentTime_;             // 現在のフェーズ内の経過時間
    int   currentWave_;

    bool isAllSpawn_; // 敵がすべていなくなったことを示すフラグ

    ///* EditorModeセット
    void SetEditorMode(bool isEditorMode);
    bool isEditorMode_ = false;

    std::string selectedEnemyType_;

    const std::string directrypath_ = "./resources/EnemyParamater/";// path
    const std::string filename_ = "PoPData.json";// name
public:
    // コンストラクタ
    EnemySpawner();

    // 初期化
    void Init();
    void Update();

    ///=======================================================================================
    /// Editor method
    ///=======================================================================================
    void CheckWaveCompletion();
	void CheckAllSpowned();

    // JSONのロード
    void LoadEnemyPoPData();

    // ImGuiによる設定
    void ImGuiUpdate();

    // セーブとロード
    void SaveAndLoad();
    void SaveEnemyPoPData();
    
    ///=======================================================================================
    /// getter method
    ///=======================================================================================
    bool GetIsAllSpawn() const { return isAllSpawn_; }

    ///=======================================================================================
   /// setter method
   ///=======================================================================================
    void SetEnemyManager(EnemyManager* enemyManager);

private:
      // JSON のロード補助関数
    void LoadPhase(Phase& phase, const json& phaseData);
    void LoadSpawn(EnemyGroup& spawn, const json& spawnData);

};