#include "RibbonTrailEditor.h"

using namespace KetaEngine;
#include "3D/RibbonTrail/RibbonTrail.h"
#include "Frame/Frame.h"
#include <cmath>
#include <imgui.h>

///============================================================
/// 初期化
///============================================================
void RibbonTrailEditor::Init(const std::string& typeName) {
    BaseEffectEditor::Init(typeName);
    previewAngle_     = 0.0f;
    emitTimer_        = 0.0f;
    prevSelectedName_ = "";
    previewTrail_     = nullptr;
}

void RibbonTrailEditor::Update(float speedRate) {
    BaseEffectEditor::Update(speedRate);

    auto* data = GetSelectedEffect();
    if (!data) {
        // 選択なし：プレビューがあれば消す
        if (previewTrail_) {
            previewTrail_->Clear();
        }
        prevSelectedName_ = "";
        return;
    }

    // 選択エフェクトが切り替わったらトレイルを再生成
    const std::string& currentName = data->GetGroupName();
    if (currentName != prevSelectedName_) {
        prevSelectedName_ = currentName;
        RebuildPreviewTrail();
    }

    if (!previewTrail_) {
        return;
    }

    // Data → Trail に毎フレーム設定を反映
    SyncPreviewTrail(data);

    // ---- 円運動でダミーポイントを追加 ----
    float deltaTime = KetaEngine::Frame::DeltaTime() * speedRate;
    previewAngle_ += deltaTime * 3.0f; // 回転速度

    // XZ平面の円運動（中心点 + 半径）
    Vector3 previewPos = {
        previewCenter_.x + std::cos(previewAngle_) * previewRadius_,
        previewCenter_.y,
        previewCenter_.z + std::sin(previewAngle_) * previewRadius_};

    emitTimer_ += deltaTime;
    if (emitTimer_ >= data->GetEmitInterval()) {
        emitTimer_ = 0.0f;
        previewTrail_->AddPoint(
            previewPos,
            data->GetStartColor(),
            data->GetStartWidth(),
            data->GetLifetime());
    }
}


void RibbonTrailEditor::RebuildPreviewTrail() {
    auto* data = GetSelectedEffect();
    if (!data) {
        return;
    }

    // 旧トレイルは RibbonTrailManager が管理しているためクリアだけする
    if (previewTrail_) {
        previewTrail_->Clear();
    }

    // 新しいトレイルを生成
    previewTrail_ = RibbonTrail::Create(static_cast<size_t>(data->GetMaxPoints()));

    SyncPreviewTrail(data);

    // プレビュー角度・タイマーをリセット
    previewAngle_ = 0.0f;
    emitTimer_    = 0.0f;
}


void RibbonTrailEditor::SyncPreviewTrail(RibbonTrailData* data) {
    if (!previewTrail_ || !data) {
        return;
    }
    previewTrail_->SetEndColor(data->GetEndColor());
    previewTrail_->SetEndWidth(data->GetEndWidth());
    previewTrail_->SetTexture(data->GetTexturePath());
    previewTrail_->SetUseDistortion(data->GetUseDistortion());
    previewTrail_->SetDistortionStrength(data->GetDistortionStrength());
    previewTrail_->SetDistortionTexture(data->GetDistortionTexturePath());
    previewTrail_->SetUVScrollSpeed(data->GetUVScrollSpeed());
    previewTrail_->SetDistortionUVScrollSpeed(data->GetDistortionUVScrollSpeed());
}

///============================================================
/// CreateEffectData
///============================================================
std::unique_ptr<RibbonTrailData> RibbonTrailEditor::CreateEffectData() {
    return std::make_unique<RibbonTrailData>();
}

///============================================================
/// RenderSpecificUI
///============================================================
void RibbonTrailEditor::RenderSpecificUI() {
    ImGui::SeparatorText("RibbonTrail Control");

    auto* data = GetSelectedEffect();
    if (!data) {
        ImGui::TextDisabled("No effect selected.");
        return;
    }

    ImGui::Text("Name: %s", data->GetGroupName().c_str());
    auto sc = data->GetStartColor();
    auto ec = data->GetEndColor();
    ImGui::Text("Start Color: (%.2f, %.2f, %.2f, %.2f)", sc.x, sc.y, sc.z, sc.w);
    ImGui::Text("End Color:   (%.2f, %.2f, %.2f, %.2f)", ec.x, ec.y, ec.z, ec.w);
    ImGui::Text("Start Width: %.3f  End Width: %.3f", data->GetStartWidth(), data->GetEndWidth());
    ImGui::Text("Lifetime: %.3f  MaxPoints: %d  Interval: %.3f",
        data->GetLifetime(), data->GetMaxPoints(), data->GetEmitInterval());
    ImGui::Text("Texture: %s",
        data->GetTexturePath().empty() ? "(default)" : data->GetTexturePath().c_str());

    // [New] プレビュー状態の表示
    ImGui::Separator();
    if (previewTrail_) {
        ImGui::TextColored({0.3f, 1.0f, 0.3f, 1.0f}, "Preview: ACTIVE (%zu points)",
            previewTrail_->GetPointCount());
    } else {
        ImGui::TextColored({1.0f, 0.4f, 0.4f, 1.0f}, "Preview: NOT ACTIVE");
    }

    // プレビュー設定
    ImGui::SeparatorText("Preview Settings");
    ImGui::DragFloat3("Center (X/Y/Z)", &previewCenter_.x, 0.1f, -50.0f, 50.0f);
    ImGui::DragFloat ("Radius",         &previewRadius_,   0.05f, 0.1f,  20.0f);

    RenderPlayBack();
}

///============================================================
/// GetFolderName
///============================================================
std::string RibbonTrailEditor::GetFolderName() const {
    return folderName_;
}

///============================================================
/// PlaySelectedAnimation
///============================================================
void RibbonTrailEditor::PlaySelectedAnimation() {
    BaseEffectEditor::PlaySelectedAnimation();

    // 再生時にプレビュートレイルもリセット
    RebuildPreviewTrail();
}
