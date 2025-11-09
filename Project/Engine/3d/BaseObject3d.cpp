#include "BaseObject3d.h"
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"
#include "ModelManager.h"

void BaseObject3d::SetModel(const std::string& modelName) {
    // モデルを検索してセット
    model_        = (ModelManager::GetInstance()->FindModel(modelName));
    textureIndex_ = model_->GetTextureIndex();

    // オブジェクトイージングアニメーションプレイヤー初期化
    if (!objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_ = std::make_unique<ObjEaseAnimationPlayer>();
        objEaseAnimationPlayer_->Init();
    }
}

void BaseObject3d::CreateMaterialResource() {
    material_.CreateMaterialResource(DirectXCommon::GetInstance());
}

///============================================================
/// WVPリソース作成
///============================================================
void BaseObject3d::CreateWVPResource() {
    wvpResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
    // データを書き込む
    wvpDate_ = nullptr;
    // 書き込むためのアドレスを取得
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
    // 単位行列を書き込んでおく
    wvpDate_->WVP                   = MakeIdentity4x4();
    wvpDate_->World                 = MakeIdentity4x4();
    wvpDate_->WorldInverseTranspose = MakeIdentity4x4();
}

///============================================================
/// WVP更新
///============================================================
void BaseObject3d::UpdateWVPData(const ViewProjection& viewProjection) {
    wvpDate_->World                 = transform_.matWorld_;
    wvpDate_->WVP                   = wvpDate_->World * viewProjection.matView_ * viewProjection.matProjection_;
    wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
}

void BaseObject3d::CreateShadowMap() {
    shadowMap_ = ShadowMap::GetInstance();
}

void BaseObject3d::DebugImgui() {
#ifdef _DEBUG
    material_.DebugImGui();
#endif
}

void BaseObject3d::SetTexture(const std::string& name) {
    textureIndex_ = TextureManager::GetInstance()->LoadTexture(textureFirePath_ + name);
}

///============================================================
/// オブジェクトイージングアニメーション再生
///============================================================
void BaseObject3d::PlayObjEaseAnimation(const std::string& categoryName, const std::string& animationName) {
    if (!objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_ = std::make_unique<ObjEaseAnimationPlayer>();
        objEaseAnimationPlayer_->Init();
    }

    objEaseAnimationPlayer_->Play(categoryName, animationName);

    // Rail使用時、親を設定
    if (objEaseAnimationPlayer_->GetAnimationData() && objEaseAnimationPlayer_->GetAnimationData()->IsUsingRail()) {
        auto* railPlayer = objEaseAnimationPlayer_->GetAnimationData()->GetRailPlayer();
        if (railPlayer) {
            railPlayer->SetParent(&transform_);
        }
    }
}

///============================================================
/// オブジェクトイージングアニメーション停止
///============================================================
void BaseObject3d::StopObjEaseAnimation() {
    if (objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_->Stop();
    }
}

///============================================================
/// アニメーション更新
///============================================================
void BaseObject3d::UpdateObjEaseAnimation(const float& deltaTime) {
    if (objEaseAnimationPlayer_) {
        objEaseAnimationPlayer_->Update(deltaTime);
        ApplyAnimationToTransform();
    }
}

///============================================================
/// アニメーション適用後のTransform更新
///============================================================
void BaseObject3d::ApplyAnimationToTransform() {
    if (!objEaseAnimationPlayer_ || !objEaseAnimationPlayer_->IsPlaying()) {
        return;
    }

    auto* animData = objEaseAnimationPlayer_->GetAnimationData();
    if (!animData) {
        return;
    }

    // Scaleを適用
    transform_.scale_ = objEaseAnimationPlayer_->GetCurrentScale();

    // Rotationを適用（ラジアン）
    transform_.rotation_ = objEaseAnimationPlayer_->GetCurrentRotation();

    // Translationを適用
    if (!animData->IsUsingRail()) {
        // 通常のイージング使用時
        transform_.translation_ = objEaseAnimationPlayer_->GetCurrentTranslation();
    } else {
        // Rail使用時
        auto* railPlayer = animData->GetRailPlayer();
        if (railPlayer) {
            transform_.translation_ = railPlayer->GetCurrentPosition();
        }
    }
}