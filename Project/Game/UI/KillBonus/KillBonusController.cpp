#include "KillBonusController.h"
#include "Audio/Audio.h"
#include <algorithm>
#include <imgui.h>

void KillBonusController::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
}

void KillBonusController::Update(float deltaTime) {
    // 同時キルウィンドウカウントダウン
    if (simKillTracker_.toleranceFrames > 0) {
        simKillTracker_.toleranceFrames--;
        if (simKillTracker_.toleranceFrames == 0) {
            FlushSimKillWindow();
        }
    }

    // エントリ更新（プレビュー中は表示時間を無限にする）
    KillBonusLayoutParam activeLayout = layout_;
    if (previewActive_) {
        activeLayout.displayDuration = 9999.0f;
    }
    for (auto& entry : entries_) {
        entry->Update(deltaTime, activeLayout);
    }

    // 終了エントリ除去
    const size_t before = entries_.size();
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
            [](const std::unique_ptr<KillBonusEntry>& e) { return e->IsFinished(); }),
        entries_.end());

    // 除去があれば目標位置を再計算
    if (entries_.size() != before) {
        RecalculateTargetPositions();
    }
}

void KillBonusController::OnEnemyKilled(int32_t comboMultiplier) {
    SpawnEntry(comboMultiplier);

    // 同時キルウィンドウに追加
    simKillTracker_.killCount++;
    simKillTracker_.entryIndices.push_back(static_cast<int>(entries_.size()) - 1);
    simKillTracker_.toleranceFrames = simKillToleranceFrames_;
}

void KillBonusController::SpawnEntry(int32_t comboMultiplier) {
    // 既存エントリを1スロット分シフト
    for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
        Vector2 target = basePos_ + Vector2(entryOffset_.x * static_cast<float>(i + 1), entryOffset_.y * static_cast<float>(i + 1));
        entries_[i]->ShiftTo(target);
    }

    // 新エントリをbasePos_に配置
    auto entry = std::make_unique<KillBonusEntry>();
    entry->Init(comboMultiplier, false, 0, basePos_);
    entries_.push_back(std::move(entry));

    KetaEngine::Audio::GetInstance()->Play("starEffect.mp3", 0.5f);
}

void KillBonusController::RecalculateTargetPositions() {
    for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
        Vector2 target = basePos_ + Vector2(entryOffset_.x * static_cast<float>(i), entryOffset_.y * static_cast<float>(i));
        entries_[i]->ShiftTo(target);
    }
}

void KillBonusController::FlushSimKillWindow() {
    if (simKillTracker_.killCount <= 1) {
        simKillTracker_.killCount = 0;
        simKillTracker_.entryIndices.clear();
        return;
    }

    // シムキルボーナス計算・コールバック
    const float bonus = simKillBonusPerKill_ * static_cast<float>(simKillTracker_.killCount);
    if (onSimKillBonusCallback_) {
        onSimKillBonusCallback_(bonus);
    }

    KetaEngine::Audio::GetInstance()->Play("fanfare.mp3", 0.7f);

    // 対象エントリにシムキル情報を遡及設定
    const int32_t bonusDisplay = static_cast<int32_t>(bonus);
    for (int idx : simKillTracker_.entryIndices) {
        if (idx >= 0 && idx < static_cast<int>(entries_.size())) {
            entries_[idx]->SetSimKill(bonusDisplay);
        }
    }

    simKillTracker_.killCount = 0;
    simKillTracker_.entryIndices.clear();
}

///=========================================================
/// プレビュー生成（SEなし・フェードなし）
///=========================================================
void KillBonusController::SpawnPreview() {
    entries_.clear();
    simKillTracker_ = {};

    auto spawnOne = [&](int32_t combo, bool hasSimKill, int32_t simKillVal) {
        for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
            Vector2 target = basePos_ + Vector2(entryOffset_.x * static_cast<float>(i + 1), entryOffset_.y * static_cast<float>(i + 1));
            entries_[i]->ShiftTo(target);
        }
        auto entry = std::make_unique<KillBonusEntry>();
        entry->Init(combo, hasSimKill, simKillVal, basePos_);
        entries_.push_back(std::move(entry));
    };

    spawnOne(previewComboMultiplier_, false, 0);
    if (previewHasSimKill_) {
        spawnOne(previewComboMultiplier_, true, previewSimKillValue_);
    }
}

///=========================================================
/// パラメータ調整
///=========================================================
void KillBonusController::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());
        ImGui::DragFloat2("基準位置", &basePos_.x, 0.1f);
        ImGui::DragFloat2("エントリーオフセット", &entryOffset_.x, 0.1f);
        ImGui::SeparatorText("レイアウト");
        ImGui::DragFloat2("回復アイコン　オフセット", &layout_.recoveryOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ倍率　オフセット", &layout_.comboOffset.x, 0.1f);
        ImGui::DragFloat2("コンボラベル オフセット", &layout_.comboLabelOffset.x, 0.1f);
        ImGui::DragFloat2("同時撃破　オフセット", &layout_.simKillOffset.x, 0.1f);
        ImGui::DragFloat2("同時撃破ラベル オフセット", &layout_.simKillLabelOffset.x, 0.1f);
        ImGui::DragFloat2("数字間隔", &layout_.digitSpacing.x, 0.1f);
        ImGui::DragFloat2("数字スケール", &layout_.digitScale.x, 0.01f);
        ImGui::DragFloat2("ベーススケール", &layout_.baseScale.x, 0.01f);
        ImGui::DragFloat("表示時間", &layout_.displayDuration, 0.01f);
        ImGui::SeparatorText("同時撃破判定");
        ImGui::InputInt("許容フレーム数", &simKillToleranceFrames_);
        ImGui::DragFloat("同時撃破ボーナス/キル", &simKillBonusPerKill_, 0.01f);
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::SeparatorText("プレビュー");
        if (ImGui::Button(previewActive_ ? "プレビュー OFF" : "プレビュー ON")) {
            previewActive_ = !previewActive_;
            if (previewActive_) {
                SpawnPreview();
            } else {
                entries_.clear();
                simKillTracker_ = {};
            }
        }
        if (previewActive_) {
            ImGui::SameLine();
            if (ImGui::Button("再生成")) {
                SpawnPreview();
            }
            ImGui::InputInt("コンボ倍率", &previewComboMultiplier_);
            ImGui::Checkbox("同時撃破あり", &previewHasSimKill_);
            if (previewHasSimKill_) {
                ImGui::InputInt("同時撃破ボーナス値", &previewSimKillValue_);
            }
        }

        ImGui::PopID();
    }
#endif
}

void KillBonusController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePos", &basePos_);
    globalParameter_->Regist(groupName_, "entryOffset", &entryOffset_);
    globalParameter_->Regist(groupName_, "recoveryOffset", &layout_.recoveryOffset);
    globalParameter_->Regist(groupName_, "comboOffset", &layout_.comboOffset);
    globalParameter_->Regist(groupName_, "comboLabelOffset", &layout_.comboLabelOffset);
    globalParameter_->Regist(groupName_, "simKillOffset", &layout_.simKillOffset);
    globalParameter_->Regist(groupName_, "simKillLabelOffset", &layout_.simKillLabelOffset);
    globalParameter_->Regist(groupName_, "digitSpacing", &layout_.digitSpacing);
    globalParameter_->Regist(groupName_, "digitScale", &layout_.digitScale);
    globalParameter_->Regist(groupName_, "baseScale", &layout_.baseScale);
    globalParameter_->Regist(groupName_, "displayDuration", &layout_.displayDuration);
    globalParameter_->Regist(groupName_, "simKillBonusPerKill", &simKillBonusPerKill_);
    globalParameter_->Regist(groupName_, "toleranceFrames", &simKillToleranceFrames_);
}
