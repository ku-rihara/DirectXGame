#include "ContinuousEnemySpawner.h"
#include "Enemy/EnemyManager.h"
#include "Field/Field.h"
#include "MathFunction.h"
#include "Player/Player.h"
#include "Random.h"
#include <cassert>
#include <imgui.h>

void ContinuousEnemySpawner::Init() {
    // グローバルパラメータ設定
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // 初期化
  
    totalSpawnedCount_ = 0;
    Start();
}

void ContinuousEnemySpawner::Update(const float& deltaTime) {
    // スポーンシステムが非アクティブなら処理しない
    if (!isActive_ || !config_.isEnabled) {
        return;
    }

    // プレイヤー情報が未設定なら処理しない
    if (!pPlayer_ || !pEnemyManager_) {
        return;
    }

    // 最大スポーン数に達していたら処理しない
    if (config_.maxSpawnCount >= 0 && totalSpawnedCount_ >= config_.maxSpawnCount) {
        return;
    }

    // タイマー更新
    spawnTimer_ += deltaTime;

    // スポーン間隔に達したか
    if (spawnTimer_ >= config_.spawnInterval) {
        spawnTimer_ = 0.0f;

        Vector3 spawnPosition = CalculateSpawnPosition();
        // 敵をスポーン
        pEnemyManager_->SpawnEnemy(config_.enemyType, spawnPosition, kContinuousSpawnGroupId);
        totalSpawnedCount_++;
    }
}

void ContinuousEnemySpawner::Start() {
    isActive_   = true;
    spawnTimer_ = 0.0f;
}

void ContinuousEnemySpawner::Stop() {
    isActive_ = false;
}

void ContinuousEnemySpawner::ResetSpawnCount() {
    totalSpawnedCount_ = 0;
}

Vector3 ContinuousEnemySpawner::CalculateSpawnPosition() {
    Vector3 spawnPos=Vector3::ZeroVector();
   
    // 複数回試行して有効な位置を探す
    for (int32_t retry = 0; retry < config_.maxRetryCount; ++retry) {
        //// プレイヤー正面方向への距離をランダムに決定
        //float forwardDistance = Random::Range(config_.forwardDistance.min, config_.forwardDistance.max);

        //// 左右のオフセットをランダムに決定
        //float lateralOffset = Random::Range(config_.lateralOffset.min, config_.lateralOffset.max);

        //　全敵の位置を取得
        std::vector<Vector3> positions;
        for (const auto& baseEnemy : pEnemyManager_->GetEnemies()) {
            Vector3 averagePos = baseEnemy->GetWorldPosition();
            positions.push_back(averagePos);
        }

        // 敵の位置の合計値を計算
        Vector3 sumPos = Vector3::ZeroVector();
        for (int32_t i = 0; i < positions.size(); ++i) {
            sumPos += positions[i];
        }

        // 平均を求める
        Vector3 averagePos = Vector3::ZeroVector();
        if (!positions.empty()) {
            averagePos = sumPos / static_cast<float>(positions.size());
        } 
        // 左右オフセットを追加
        spawnPos = averagePos;

        // フィールド境界内に補正
        if (!ClampToFieldBounds(spawnPos)) {
            continue; // フィールド外だったので再試行
        }

        // 他の敵との距離が近いか判定
        if (IsValidSpawnPosition(spawnPos)) {
            return spawnPos;
        }
    }

    return Vector3::ZeroVector();
}

bool ContinuousEnemySpawner::IsValidSpawnPosition(const Vector3& position) const {
    if (!pEnemyManager_) {
        return true; // チェック不可なら許可
    }

    // 既存の敵との距離チェック
    for (const auto& enemy : pEnemyManager_->GetEnemies()) {
        float dx     = position.x - enemy->GetWorldPosition().x;
        float dz     = position.z - enemy->GetWorldPosition().z;
        float length = Vector2(dx, dz).Length();

        if (length < config_.minDistanceFromEnemies) {
            return false; // 他の敵が近すぎる
        }
    }

    return true;
}

bool ContinuousEnemySpawner::ClampToFieldBounds(Vector3& position) const {

    // フィールドの中心とスケールを取得
    Vector3 fieldCenter = {0.0f, 0.0f, 0.0f}; // フィールド中心
    Vector3 fieldScale  = Field::baseScale_; // フィールドのスケール
    // フィールドの範囲を計算
    float halfScaleX = fieldScale.x;
    float halfScaleZ = fieldScale.z;

    float minX = fieldCenter.x - halfScaleX;
    float maxX = fieldCenter.x + halfScaleX;
    float minZ = fieldCenter.z - halfScaleZ;
    float maxZ = fieldCenter.z + halfScaleZ;

    // 範囲外かチェック
    bool wasOutside = false;

    if (position.x < minX || position.x > maxX || position.z < minZ || position.z > maxZ) {
        wasOutside = true;
    }

    // 範囲内に制限
    position.x = Clamp(position.x, minX + config_.fieldMargin, maxX - config_.fieldMargin);
    position.z = Clamp(position.z, minZ + config_.fieldMargin, maxZ - config_.fieldMargin);

    // 元々範囲外だった場合はfalseを返す
    return !wasOutside;
}

///=========================================================
/// パラメータ調整
///=========================================================
void ContinuousEnemySpawner::AdjustParam() {
#ifdef _DEBUG

    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // 基本設定
        ImGui::SeparatorText("Basic Settings");
        ImGui::Checkbox("Enable Continuous Spawn", &config_.isEnabled);
        ImGui::DragFloat("Spawn Interval (sec)", &config_.spawnInterval, 0.1f);
        ImGui::InputInt("Max Spawn Count (-1=Unlimited)", &config_.maxSpawnCount);

        // 距離設定
        ImGui::SeparatorText("Forward Distance Settings");
        ImGui::DragFloat("Forward Distance Min", &config_.forwardDistance.min, 0.1f);
        ImGui::DragFloat("Forward Distance Max", &config_.forwardDistance.max, 0.1f);

        // 左右オフセット設定
        ImGui::SeparatorText("Lateral Offset Settings");
        ImGui::DragFloat("Lateral Offset Min", &config_.lateralOffset.min, 0.1f);
        ImGui::DragFloat("Lateral Offset Max", &config_.lateralOffset.max, 0.1f);

        // その他設定
        ImGui::SeparatorText("Other Settings");
        ImGui::DragFloat("Min Distance from Enemies", &config_.minDistanceFromEnemies, 0.1f);
        ImGui::DragFloat("fieldMargin", &config_.fieldMargin, 0.1f);
        ImGui::InputInt("Max Retry Count", &config_.maxRetryCount);

        // 制御ボタン
        if (ImGui::Button("Start")) {
            Start();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            Stop();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Count")) {
            ResetSpawnCount();
        }

        // セーブ・ロード
        ImGui::Separator();
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }

#endif
}

void ContinuousEnemySpawner::RegisterParams() {
    globalParameter_->Regist(groupName_, "enabled", &config_.isEnabled);
    globalParameter_->Regist(groupName_, "spawnInterval", &config_.spawnInterval);
    globalParameter_->Regist(groupName_, "forwardDistanceMin", &config_.forwardDistance.min);
    globalParameter_->Regist(groupName_, "forwardDistanceMax", &config_.forwardDistance.max);
    globalParameter_->Regist(groupName_, "lateralOffsetMin", &config_.lateralOffset.min);
    globalParameter_->Regist(groupName_, "lateralOffsetMax", &config_.lateralOffset.max);
    globalParameter_->Regist(groupName_, "minDistanceFromEnemies", &config_.minDistanceFromEnemies);
    globalParameter_->Regist(groupName_, "maxRetryCount", &config_.maxRetryCount);
    globalParameter_->Regist(groupName_, "maxSpawnCount", &config_.maxSpawnCount);
    globalParameter_->Regist(groupName_, "fieldMargin", &config_.fieldMargin);
}