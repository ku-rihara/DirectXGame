#include "Object3d.h"
#include "Lighrt/DirectionalLight.h"
#include "Lighrt/Light.h"
#include "ModelManager.h"
#include "Pipeline/SkinningObject3DPipeline.h"
// #include"struct/ModelData.h"

///============================================================
/// モデル作成
///============================================================
Object3d* Object3d::CreateModel(const std::string& instanceName) {
    // 新しいModelインスタンスを作成
    Object3d* object3d = new Object3d();
    ModelManager::GetInstance()->LoadModel(instanceName);
    object3d->SetModel(instanceName);
    object3d->CreateWVPResource();
    object3d->CreateShadowMap();
    object3d->CreateMaterialResource();
    return object3d;
}

///============================================================
/// 初期化
///============================================================
void Object3d::Init() {
}

///============================================================
/// 更新
///============================================================
void Object3d::ColorUpdate() {

    material_.materialData_->color = objColor_.GetColor();
}

///============================================================
/// 描画
///============================================================
void Object3d::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    if (!model_)
        return;

    ColorUpdate();
    shadowMap_->UpdateLightMatrix();

    // WVP行列の計算
    wvpDate_->World                 = worldTransform.matWorld_;
    wvpDate_->WVP                   = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
    wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));

    // 影の更新

    Object3DPiprline::GetInstance()->PreBlendSet(DirectXCommon::GetInstance()->GetCommandList(), blendMode);

    model_->Draw(wvpResource_, *shadowMap_, material_, textureHandle);
}

///============================================================
/// 描画
///============================================================
void Object3d::DrawAnimation(const WorldTransform& worldTransform, const ViewProjection& viewProjection, SkinCluster skinCluster, std::optional<uint32_t> textureHandle) {
    if (!model_)
        return;

    ColorUpdate();

    wvpDate_->World                 = worldTransform.matWorld_;
    wvpDate_->WVP                   = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
    wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
    /*}*/

    SkinningObject3DPipeline::GetInstance()->PreBlendSet(DirectXCommon::GetInstance()->GetCommandList(), blendMode);
    model_->DrawAnimation(wvpResource_, *shadowMap_, material_, skinCluster, textureHandle);

    ShadowDraw();
}

void Object3d::ShadowDraw() {

    /* if (!isShadow_) {
         return;
     }*/

    if (!model_) {
        return;
    }

    // モデルの影描画を呼び出し
    model_->DrawForShadowMap(wvpResource_, *shadowMap_);
}

///============================================================
/// デバッグ表示
///============================================================

void Object3d::DebugImgui() {
    BaseObject3d::DebugImgui();
}

///============================================================
/// WVPリソース作成
///============================================================
void Object3d::CreateWVPResource() {
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

void Object3d::CreateShadowMap() {
    shadowMap_ = ShadowMap::GetInstance();
}

///============================================================
/// マテリアルリソース作成
///============================================================
void Object3d::CreateMaterialResource() {
    BaseObject3d::CreateMaterialResource();
}