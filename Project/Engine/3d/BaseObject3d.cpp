#include "BaseObject3d.h"

using namespace KetaEngine;
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"
#include "ModelManager.h"

void BaseObject3d::SetModelByName(const std::string& modelName) {
    // モデルを検索してセット
    model_        = (ModelManager::GetInstance()->FindModel(modelName));
    textureIndex_ = model_->GetTextureIndex();
}

void BaseObject3d::CreateMaterialResource() {
    material_.Init(DirectXCommon::GetInstance());
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

void BaseObject3d::DebugImGui() {
#ifdef _DEBUG
    material_.DebugImGui();
#endif
}

void BaseObject3d::SetTextureByName(const std::string& name) {
    textureIndex_ = TextureManager::GetInstance()->LoadTexture(textureFirePath_ + name);
}

