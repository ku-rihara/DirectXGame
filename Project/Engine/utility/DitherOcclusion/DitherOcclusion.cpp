#include "DitherOcclusion.h"
#include "Base/Material/ModelMaterial.h"
#include "Base/WinApp.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Light/Light.h"
#include "math/Matrix4x4.h"
#include "math/Vector4.h"
#include <imgui.h>

using namespace KetaEngine;

void DitherOcclusion::Init() {
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(kGroupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(kGroupName_);
}

void DitherOcclusion::RegisterParams() {
    globalParameter_->Regist(kGroupName_, "ScreenRadius", &screenRadius_);
    globalParameter_->Regist(kGroupName_, "DitherAlpha", &ditherAlpha_);
}

void DitherOcclusion::Add(ModelMaterial* material) {
    if (!material) {
        return; 
    }

    material->SetEnableDither(true);
}

void DitherOcclusion::Update(
    const ViewProjection& vp,
    const Vector3&        playerWorldPos)
{
    // ビュープロジェクション行列でプレイヤーをクリップ空間へ変換
    Matrix4x4 vpMatrix = vp.matView_ * vp.matProjection_;
    Vector4 clipPos = TransformMatrix(Vector4(playerWorldPos.x, playerWorldPos.y, playerWorldPos.z, 1.0f), vpMatrix);

    PlayerOcclusionData data{};

    if (clipPos.w > 0.0f) {
        float invW = 1.0f / clipPos.w;
        float ndcX = clipPos.x * invW;
        float ndcY = clipPos.y * invW;
        float ndcZ = clipPos.z * invW;

        // NDC → スクリーンピクセル座標
        data.screenPos.x  = (ndcX + 1.0f) * 0.5f * static_cast<float>(WinApp::kWindowWidth);
        data.screenPos.y  = (1.0f - ndcY) * 0.5f * static_cast<float>(WinApp::kWindowHeight);
        data.depth        = ndcZ;
        data.screenRadius = screenRadius_;
        data.ditherAlpha  = ditherAlpha_;
        data.enabled      = 1;
    } else {
        data.enabled = 0;
    }

    Light::GetInstance()->SetPlayerOcclusion(data);
}

void DitherOcclusion::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(kGroupName_.c_str())) {
        ImGui::PushID(kGroupName_.c_str());

        ImGui::DragFloat("ScreenRadius", &screenRadius_, 1.0f,  1.0f, 600.0f);
        ImGui::DragFloat("DitherAlpha",  &ditherAlpha_,  0.01f, 0.0f,   1.0f);

      
        globalParameter_->ParamSaveForImGui(kGroupName_);
        globalParameter_->ParamLoadForImGui(kGroupName_);

        ImGui::PopID();
    }
#endif
}
