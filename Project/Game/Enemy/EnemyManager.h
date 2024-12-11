#pragma once
#include "NormalEnemy.h"
#include "3d/ViewProjection.h"
#include <vector>
#include <memory>
#include <string>
#include <imgui.h>

#include "EnemyManager.h"
#include <format>
#include <fstream>
#include <json.hpp>

class EnemyManager {
private:
    using json = nlohmann::json;
private:
    ///========================================================================================
    ///  private variants
    ///========================================================================================
  
    /// リスト
    std::vector<std::unique_ptr<BaseEnemy>> enemies_;
    std::vector<std::string> enemyTypes_ = { "NormalEnemy" };

    // 一時的な敵生成用データ
    std::string selectedEnemyType_;                        // 種類の名前
    Vector3 spownPosition_;                                // 発生位置
    float spownTime_;                                      // 発生までの時間
    std::vector<std::pair<float, Vector3>> spownSchedule_; // 時間と位置のペア

    float currentTime_; /// 現在時間

public:
    // コンストラクタ
    EnemyManager();

    ///========================================================================================
    ///  public method
    ///========================================================================================

    void Init();

    // 敵の生成
    void SpawnEnemy(const Vector3& spawnPos);

    // 更新処理
    void Update();

    // 描画処理
    void Draw(const ViewProjection& viewProjection);

    // スプライト描画処理
    void SpriteDraw(const ViewProjection& viewProjection);

    // ImGuiによるエディタ
    void ImGuiUpdate();
};
