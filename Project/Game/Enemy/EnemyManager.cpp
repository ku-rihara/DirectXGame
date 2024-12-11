#include "EnemyManager.h"
#include "Frame/Frame.h"

EnemyManager::EnemyManager() {
	
}

///========================================================================================
///  初期化
///========================================================================================

void EnemyManager::Init() {

	 selectedEnemyType_ = "NormalEnemy";
	 spownPosition_ = {};
	 spownTime_ = 0.0f;
}

///========================================================================================
///  敵の生成
///========================================================================================
void EnemyManager::SpawnEnemy(const Vector3& spawnPos) {
    std::unique_ptr<BaseEnemy> enemy;

    if (selectedEnemyType_ == "NormalEnemy") {
        enemy = std::make_unique<NormalEnemy>();
    }
    // 他の敵タイプもここに追加可能

    enemy->Init(spawnPos);
    enemies_.emplace_back(std::move(enemy));
}

///========================================================================================
///  更新処理
///========================================================================================
void EnemyManager::Update() {

    // 発生時間によって発生させる
    currentTime_ += Frame::DeltaTime();
    for (auto it = spownSchedule_.begin(); it != spownSchedule_.end();++it ) {

        if (currentTime_ >= it->first) {// 時間超えたら生成
            SpawnEnemy(it->second);
            it = spownSchedule_.erase(it);
        }
        else {
            ++it;// 次の要素
        }
    }

    /// 更新処理
    for (auto& enemy : enemies_) {
        enemy->Update();
    }
}

///========================================================================================
///  描画処理
///========================================================================================
void EnemyManager::Draw(const ViewProjection& viewProjection) {
    for (auto& enemy : enemies_) {
        enemy->Draw(viewProjection);
    }
}

///========================================================================================
///  スプライト描画処理
///========================================================================================
void EnemyManager::SpriteDraw(const ViewProjection& viewProjection) {
    for (auto& enemy : enemies_) {
        enemy->SpriteDraw(viewProjection);
    }
}

///========================================================================================
///  ImGuiによるエディタ
///========================================================================================
void EnemyManager::ImGuiUpdate() {
#ifdef _DEBUG

    ImGui::Begin("Enemy Manager");/// begin

    /// 敵の種類選択
    if (ImGui::BeginCombo("Enemy Type", selectedEnemyType_.c_str())) {
        for (const auto& type : enemyTypes_) {
            if (ImGui::Selectable(type.c_str(), type == selectedEnemyType_)) {
                selectedEnemyType_ = type;
            }
        }
        ImGui::EndCombo();
    }

#endif // _DEBUG
}
