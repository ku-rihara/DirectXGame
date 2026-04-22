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

void DitherOcclusion::Add(ModelMaterial* material) {
    if (!material) {
        return;
    }

    // ディザ抜きを有効
    material->SetEnableDither(true);
}

void DitherOcclusion::Update(
    const ViewProjection& vp,
    const Vector3& playerWorldPos) {
    PlayerOcclusionData data{};

    // ワールド座標をスクリーン座標に変換
    const Vector3 screen = ScreenTransformWithDepth(playerWorldPos, vp);

    if (screen.z >= 0.0f && screen.z <= 1.0f) {
        data.screenPos.x  = screen.x;
        data.screenPos.y  = screen.y;
        data.depth        = screen.z;
        data.screenRadius = screenRadius_;
        data.ditherAlpha  = ditherAlpha_;
        data.enabled      = 1;
    } else {
        data.enabled = 0;
    }

    Light::GetInstance()->SetPlayerOcclusion(data);
}

void DitherOcclusion::RegisterParams() {
    globalParameter_->Regist(kGroupName_, "ScreenRadius", &screenRadius_);
    globalParameter_->Regist(kGroupName_, "DitherAlpha", &ditherAlpha_);
}

void DitherOcclusion::AdjustParam() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader(kGroupName_.c_str())) {
        ImGui::PushID(kGroupName_.c_str());

        ImGui::DragFloat("ScreenRadius", &screenRadius_, 1.0f);
        ImGui::DragFloat("DitherAlpha", &ditherAlpha_, 0.01f, 0.0f, 1.0f);

        globalParameter_->ParamSaveForImGui(kGroupName_);
        globalParameter_->ParamLoadForImGui(kGroupName_);

        ImGui::PopID();
    }
#endif
}
