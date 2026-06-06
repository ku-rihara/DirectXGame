#include "BossAttackBombManager.h"
#include "Frame/Frame.h"
#include <imgui.h>

void BossAttackBombManager::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void BossAttackBombManager::Update() {

    if (!isActive_) {
        return;
    }

    // 投げる間隔タイマーの更新
    throwTimer_ += KetaEngine::Frame::DeltaTimeRate();

    // 投げる時間になったら投げる
    if (throwTimer_ >= bombThrowInterval_) {
        throwTimer_ = 0.0f;
        ThrowBomb();
    }

    for (auto it = bombs_.begin(); it != bombs_.end();) {
        // ターゲット位置の更新
        (*it)->SetTargetPosition(playerPos_);
        // ボムの更新
        (*it)->Update();
        // 着地したらコールバック呼ぶ
        if ((*it)->IsLanded()) {
            it = bombs_.erase(it);
        } else {
            ++it;
        }
    }
}

void BossAttackBombManager::Start() {
    // 開始直後に1発投げる
    isActive_   = true;
    throwTimer_ = bombThrowInterval_;
    bombs_.clear();
}

void BossAttackBombManager::Stop() {
    // ボムを全て消す
    isActive_ = false;
    bombs_.clear();
}

void BossAttackBombManager::ThrowBomb() {

    // ボムの生成と初期化
    auto bomb = std::make_unique<AttackBomb>();
    bomb->Init(bossPos_, playerPos_, flightTime_, arcHeight_);

    // 着地コールバックの設定
    float stressAmount = stressPerBomb_;
    bomb->SetOnLandedCallback([this, stressAmount]() {
        if (onBombLanded_) {
            onBombLanded_(stressAmount);
        }
    });

    // ボムを管理リストに追加
    bombs_.emplace_back(std::move(bomb));
}

void BossAttackBombManager::RegisterParams() {
    globalParameter_->Regist(groupName_, "bombThrowInterval", &bombThrowInterval_);
    globalParameter_->Regist(groupName_, "stressPerBomb", &stressPerBomb_);
    globalParameter_->Regist(groupName_, "arcHeight", &arcHeight_);
    globalParameter_->Regist(groupName_, "flightTime", &flightTime_);
}

void BossAttackBombManager::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("爆弾投げ間隔（秒）", &bombThrowInterval_, 0.1f);
        ImGui::DragFloat("着地時ストレス増加量", &stressPerBomb_, 0.1f);
        ImGui::DragFloat("放物線の高さ", &arcHeight_, 0.1f);
        ImGui::DragFloat("飛行時間（秒）", &flightTime_, 0.05f);

        ImGui::Text("飛行中のボム数: %zu", bombs_.size());

        if (!globalParameter_->HasRegisters(groupName_)) {
            RegisterParams();
        }

        // セーブ、ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
