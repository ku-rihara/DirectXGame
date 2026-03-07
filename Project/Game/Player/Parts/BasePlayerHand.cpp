#include "BasePlayerHand.h"
#include "Editor/ObjEaseAnimation/ObjEaseAnimationPlayer.h"
/// imGui
#include "base/TextureManager.h"
#include <imgui.h>
///=========================================================
/// 　初期化
///==========================================================
void BasePlayerHand::Init() {

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    particlePlayer_ = std::make_unique<KetaEngine::ParticlePlayer>();
    particlePlayer_->Init();
    particlePlayer_->SetFollowingPos(&effectFollowPos_);

    // トレイルプレイヤー初期化
    trailPlayer_.Init();
    trailPlayer_.SetFollowPosition(&effectFollowPos_);

    // ディゾルブエッジ設定
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeColor = Vector3(0.6706f, 0.8824f, 0.9804f);
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveEdgeWidth = 0.05f;
    dissolvePlayer_.Init();
}

///=========================================================
/// 　更新
///==========================================================
void BasePlayerHand::Update() {

    // 戻り中は進行方向の逆を向くよう WorldTransform に伝える
    auto* animPlayer = obj3d_->transform_.GetObjEaseAnimationPlayer();
    obj3d_->transform_.SetReverseDirectionOnReturn(animPlayer->IsTranslationReturning());

    obj3d_->SetIsShadow(isShadow_);

    // エミッター更新
    Vector3 handPos  = obj3d_->transform_.GetWorldPos();
    particlePlayer_->SetTargetPosition(handPos);
    effectFollowPos_ = handPos;
    particlePlayer_->Update();

    // ディゾルブ更新・適用
    dissolvePlayer_.Update();
    if (dissolvePlayer_.IsPlaying()) {
        dissolvePlayer_.ApplyToMaterial(*obj3d_->GetModelMaterial());
    }

    // トレイル更新
    trailPlayer_.Update();

    BaseObject::Update();
}

///=================================================================================
/// パラメータ登録
///=================================================================================
void BasePlayerHand::RegisterParams() {
    globalParameter_->Regist(groupName_, "Translate", &obj3d_->transform_.translation_);
}

void BasePlayerHand::AdjustParamBase() {
#ifdef _DEBUG
    ImGui::SeparatorText("Param");
    ImGui::DragFloat3("Position", &obj3d_->transform_.translation_.x, 0.1f);
    globalParameter_->ParamSaveForImGui(groupName_);
    globalParameter_->ParamLoadForImGui(groupName_);
#endif // _DEBUG
}


void BasePlayerHand::PlayDissolve(const std::string& name) {
    dissolvePlayer_.Play(name);
}

void BasePlayerHand::SetInitialDissolveHidden() {
    obj3d_->GetModelMaterial()->GetMaterialData()->enableDissolve    = true;
    obj3d_->GetModelMaterial()->GetMaterialData()->dissolveThreshold = 1.0f;
}

void BasePlayerHand::SetParent(KetaEngine::WorldTransform* parent) {
    obj3d_->transform_.parent_ = parent;
}

void BasePlayerHand::StartTrailEmit(const std::string& presetName, const std::string& category) {
    trailPlayer_.Play(presetName, category);
}

void BasePlayerHand::EffectEmit(const std::string& effectName) {
    if (isEmit_) {
        particlePlayer_->Play(effectName,"Player");
    }
}