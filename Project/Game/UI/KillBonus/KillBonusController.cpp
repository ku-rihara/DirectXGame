#include "KillBonusController.h"
#include "Audio/Audio.h"
#include <imgui.h>

void KillBonusController::Init() {
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    comboUI_.Init();
    simKillUI_.Init();
}

void KillBonusController::Update(float deltaTime) {
    // 同時キルウィンドウカウントダウン
    if (simKillTracker_.toleranceTime > 0.0f) {
        simKillTracker_.toleranceTime -= deltaTime;
        if (simKillTracker_.toleranceTime <= 0.0f) {
            simKillTracker_.toleranceTime = 0.0f;
            FlushSimKillWindow();
        }
    }

    // 共有表示タイマー
    if (displayActive_ && !previewActive_) {
        displayTimer_ -= deltaTime;
        if (displayTimer_ <= 0.0f) {
            displayActive_ = false;
            comboUI_.Close();
            simKillUI_.Close();
        }
    }

    // コンボUIの段階カラー
    for (int i = 0; i < 3; ++i) {
        comboLayout_.colorTiers[i].color = simKillLayout_.colorTiers[i].color;
    }

    comboUI_.Update(deltaTime, comboLayout_);
    simKillUI_.Update(deltaTime, simKillLayout_);
}

void KillBonusController::OnEnemyKilled([[maybe_unused]] int32_t comboCount) {
    simKillTracker_.killCount++;
    simKillTracker_.toleranceTime = simKillToleranceTime_;
}

void KillBonusController::OnBonusFlyArrived(float comboBonusValue, int32_t comboCount) {
    lastComboBonusValue_ = comboBonusValue;
    comboUI_.Spawn(lastComboBonusValue_, comboCount);

    // 保留中の同時キルがあれば同タイミングでSpawn
    if (pendingSimKillCount_ > 1) {
        simKillUI_.Spawn(pendingSimKillCount_, lastComboBonusValue_);
        pendingSimKillCount_ = 0;
    }

    ResetDisplayTimer();

    KetaEngine::Audio::GetInstance()->Play("TimerCure.mp3", 0.5f);
}

void KillBonusController::FlushSimKillWindow() {
    const int32_t killCount   = simKillTracker_.killCount;
    simKillTracker_.killCount = 0;

    if (killCount <= 1) {
        return;
    }

    // ボーナスコールバック
    const float bonus = simKillBonusPerKill_ * static_cast<float>(killCount);
    if (onSimKillBonusCallback_) {
        onSimKillBonusCallback_(bonus);
    }

    // fly到着まで保留
    pendingSimKillCount_ = killCount;
}

void KillBonusController::ResetDisplayTimer() {
    displayTimer_  = displayDuration_;
    displayActive_ = true;
}

///=========================================================
/// デバッグプレビュー
///=========================================================
void KillBonusController::SpawnPreview() {
    comboUI_.SetShowMaxDigits(true);
    simKillUI_.SetShowMaxDigits(true);
    comboUI_.Spawn(previewComboBonusValue_, previewSimKillCount_);
    if (previewHasSimKill_) {
        simKillUI_.Spawn(previewSimKillCount_, previewComboBonusValue_);
    }
    ResetDisplayTimer();
}

void KillBonusController::RegisterParams() {
    globalParameter_->Regist(groupName_, "displayDuration", &displayDuration_);
    globalParameter_->Regist(groupName_, "simKillToleranceTime", &simKillToleranceTime_);
    globalParameter_->Regist(groupName_, "simKillBonusPerKill", &simKillBonusPerKill_);

    // コンボUI
    globalParameter_->Regist(groupName_, "combo_basePos", &comboLayout_.basePos);
    globalParameter_->Regist(groupName_, "combo_bgOffset", &comboLayout_.bgOffset);
    globalParameter_->Regist(groupName_, "combo_bgScale", &comboLayout_.bgScale);
    globalParameter_->Regist(groupName_, "combo_bgColor", &comboLayout_.bgColor);
    globalParameter_->Regist(groupName_, "combo_labelOffset", &comboLayout_.labelOffset);
    globalParameter_->Regist(groupName_, "combo_comboTextOffset", &comboLayout_.comboTextOffset);
    globalParameter_->Regist(groupName_, "combo_digitOffset", &comboLayout_.digitOffset);
    globalParameter_->Regist(groupName_, "combo_decimalPointOffset", &comboLayout_.decimalPointOffset);
    globalParameter_->Regist(groupName_, "combo_decimalDigitOffset", &comboLayout_.decimalDigitOffset);
    globalParameter_->Regist(groupName_, "combo_multiplyIconOffset", &comboLayout_.multiplyIconOffset);
    globalParameter_->Regist(groupName_, "combo_digitSpacing", &comboLayout_.digitSpacing);
    globalParameter_->Regist(groupName_, "combo_digitScale", &comboLayout_.digitScale);
    globalParameter_->Regist(groupName_, "combo_leftParenOffset", &comboLayout_.leftParenOffset);
    globalParameter_->Regist(groupName_, "combo_rightParenOffset", &comboLayout_.rightParenOffset);
    globalParameter_->Regist(groupName_, "combo_comboCountOffset", &comboLayout_.comboCountOffset);
    globalParameter_->Regist(groupName_, "combo_comboCountSpacing", &comboLayout_.comboCountSpacing);
    globalParameter_->Regist(groupName_, "combo_comboCountScale", &comboLayout_.comboCountScale);

    // 同時キルUI
    globalParameter_->Regist(groupName_, "simKill_basePos", &simKillLayout_.basePos);
    globalParameter_->Regist(groupName_, "simKill_labelOffset", &simKillLayout_.labelOffset);
    globalParameter_->Regist(groupName_, "simKill_digitOffset", &simKillLayout_.digitOffset);
    globalParameter_->Regist(groupName_, "simKill_digitSpacing", &simKillLayout_.digitSpacing);
    globalParameter_->Regist(groupName_, "simKill_digitScale", &simKillLayout_.digitScale);
    globalParameter_->Regist(groupName_, "simKill_bodyOffset", &simKillLayout_.bodyOffset);
    globalParameter_->Regist(groupName_, "simKill_leftParenOffset", &simKillLayout_.leftParenOffset);
    globalParameter_->Regist(groupName_, "simKill_rightParenOffset", &simKillLayout_.rightParenOffset);
    globalParameter_->Regist(groupName_, "simKill_comboDigitOffset", &simKillLayout_.comboDigitOffset);
    globalParameter_->Regist(groupName_, "simKill_comboDecimalPointOffset", &simKillLayout_.comboDecimalPointOffset);
    globalParameter_->Regist(groupName_, "simKill_comboDecimalDigitOffset", &simKillLayout_.comboDecimalDigitOffset);
    globalParameter_->Regist(groupName_, "simKill_comboDecimalSpacing", &simKillLayout_.comboDecimalSpacing);
    globalParameter_->Regist(groupName_, "simKill_comboDigitScale", &simKillLayout_.comboDigitScale);
    globalParameter_->Regist(groupName_, "simKill_multiplyIconOffset", &simKillLayout_.multiplyIconOffset);

    // 段階カラー
    for (int i = 0; i < 3; ++i) {
        const std::string prefix = "simKill_tier" + std::to_string(i);
        globalParameter_->Regist(groupName_, prefix + "_threshold", &simKillLayout_.colorTiers[i].threshold);
        globalParameter_->Regist(groupName_, prefix + "_color", &simKillLayout_.colorTiers[i].color);
    }

    // 段階カラー
    for (int i = 0; i < 3; ++i) {
        const std::string prefix = "combo_tier" + std::to_string(i);
        globalParameter_->Regist(groupName_, prefix + "_threshold", &comboLayout_.colorTiers[i].threshold);
    }
}

///=========================================================
/// パラメータ調整
///=========================================================
void KillBonusController::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat("表示時間", &displayDuration_, 0.01f, 0.0f, 10.0f);
        ImGui::SeparatorText("同時キル判定");
        ImGui::DragFloat("同時キル判定時間(秒)", &simKillToleranceTime_, 0.01f, 0.0f, 2.0f);
        ImGui::DragFloat("同時撃破ボーナス/キル", &simKillBonusPerKill_, 0.01f);

        ImGui::SeparatorText("コンボUI レイアウト");
        ImGui::DragFloat2("コンボ 基準位置", &comboLayout_.basePos.x, 0.1f);
        ImGui::DragFloat2("コンボ BG オフセット", &comboLayout_.bgOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ BG スケール", &comboLayout_.bgScale.x, 0.01f);
        ImGui::ColorEdit4("コンボ BG カラー", &comboLayout_.bgColor.x);
        ImGui::DragFloat2("コンボ ラベル オフセット", &comboLayout_.labelOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ テキスト オフセット", &comboLayout_.comboTextOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ MultiplyIcon オフセット", &comboLayout_.multiplyIconOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ 整数桁 オフセット", &comboLayout_.digitOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ 小数点 オフセット", &comboLayout_.decimalPointOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ 小数桁 オフセット", &comboLayout_.decimalDigitOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ 小数桁間隔", &comboLayout_.digitSpacing.x, 0.1f);
        ImGui::DragFloat2("コンボ 数字スケール", &comboLayout_.digitScale.x, 0.01f);
        ImGui::DragFloat2("コンボ 左カッコ オフセット", &comboLayout_.leftParenOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ 右カッコ オフセット", &comboLayout_.rightParenOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ数 オフセット", &comboLayout_.comboCountOffset.x, 0.1f);
        ImGui::DragFloat2("コンボ数 桁間隔", &comboLayout_.comboCountSpacing.x, 0.1f);
        ImGui::DragFloat2("コンボ数 スケール", &comboLayout_.comboCountScale.x, 0.01f);

        ImGui::SeparatorText("同時キルUI レイアウト");
        ImGui::DragFloat2("同時キル 基準位置", &simKillLayout_.basePos.x, 0.1f);
        ImGui::DragFloat2("同時キル ラベル オフセット", &simKillLayout_.labelOffset.x, 0.1f);
        ImGui::DragFloat2("同時キル 数字 オフセット", &simKillLayout_.digitOffset.x, 0.1f);
        ImGui::DragFloat2("同時キル 数字間隔", &simKillLayout_.digitSpacing.x, 0.1f);
        ImGui::DragFloat2("同時キル 数字スケール", &simKillLayout_.digitScale.x, 0.01f);
        ImGui::DragFloat2("同時キル 体 オフセット", &simKillLayout_.bodyOffset.x, 0.1f);
        ImGui::DragFloat2("左カッコ オフセット", &simKillLayout_.leftParenOffset.x, 0.1f);
        ImGui::DragFloat2("右カッコ オフセット", &simKillLayout_.rightParenOffset.x, 0.1f);
        ImGui::DragFloat2("倍率 整数桁 オフセット", &simKillLayout_.comboDigitOffset.x, 0.1f);
        ImGui::DragFloat2("倍率 小数点 オフセット", &simKillLayout_.comboDecimalPointOffset.x, 0.1f);
        ImGui::DragFloat2("倍率 小数桁 オフセット", &simKillLayout_.comboDecimalDigitOffset.x, 0.1f);
        ImGui::DragFloat2("倍率 小数桁間隔", &simKillLayout_.comboDecimalSpacing.x, 0.1f);
        ImGui::DragFloat2("倍率 数字スケール", &simKillLayout_.comboDigitScale.x, 0.01f);
        ImGui::DragFloat2("同時キル MultiplyIcon オフセット", &simKillLayout_.multiplyIconOffset.x, 0.1f);

        ImGui::SeparatorText("コンボ 段階カラー閾値（色は同時キルと共有）");
        for (int i = 0; i < 3; ++i) {
            ImGui::PushID(i);
            ImGui::InputInt("閾値(コンボ数)", &comboLayout_.colorTiers[i].threshold);
            ImGui::PopID();
        }

        ImGui::SeparatorText("同時キル 段階カラー");
        for (int i = 0; i < 3; ++i) {
            ImGui::PushID(i);
            ImGui::InputInt("閾値(キル数)", &simKillLayout_.colorTiers[i].threshold);
            ImGui::ColorEdit4("カラー", &simKillLayout_.colorTiers[i].color.x);
            ImGui::PopID();
        }

        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::SeparatorText("プレビュー");
        if (ImGui::Button(previewActive_ ? "プレビュー OFF" : "プレビュー ON")) {
            previewActive_ = !previewActive_;
            if (previewActive_) {
                SpawnPreview();
            } else {
                comboUI_.SetShowMaxDigits(false);
                simKillUI_.SetShowMaxDigits(false);
            }
        }
        if (previewActive_) {
            ImGui::SameLine();
            if (ImGui::Button("再生成")) {
                SpawnPreview();
            }
            ImGui::DragFloat("コンボ倍率", &previewComboBonusValue_, 0.01f, 1.0f, 99.99f);
            ImGui::Checkbox("同時キルあり", &previewHasSimKill_);
            if (previewHasSimKill_) {
                ImGui::InputInt("同時キル数", &previewSimKillCount_);
            }
        }

        ImGui::PopID();
    }
#endif
}
