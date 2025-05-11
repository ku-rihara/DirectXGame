#include"IPrimitive.h"
#include"base/DirectXCommon.h"

void IPrimitive::Init() {

    mesh_ = std::make_unique<Mesh>();
    CreateWVPResource();
    CreateMaterialResource();
    mesh_->Init(DirectXCommon::GetInstance(), vertexNum_);
    Create();
}

///============================================================
/// WVPリソース作成
///============================================================
void IPrimitive::CreateWVPResource() {
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
/// 描画
///============================================================
void IPrimitive::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    if (!mesh_)
        return;

    wvpDate_->World                 = worldTransform.matWorld_;
    wvpDate_->WVP                   = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
    wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
    
    mesh_->Draw(wvpResource_, material_, textureHandle);
}

void IPrimitive::SetTexture(const std::string& name) {
    mesh_->SetTexture(name);
}

 void IPrimitive::CreateMaterialResource() {
    material_.CreateMaterialResource(DirectXCommon::GetInstance());
     material_.SetShininess(1.0f);

 }