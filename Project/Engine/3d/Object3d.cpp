#include "Object3d.h"
#include "ModelManager.h"
#include "Pipeline/SkinningObject3DPipeline.h"
#include"Lighrt/Light.h"
#include"Lighrt/DirectionalLight.h"
// #include"struct/ModelData.h"

Object3d::Object3d() {
}
Object3d::~Object3d() {
}

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

    // WVP行列の計算
    wvpDate_->World                 = worldTransform.matWorld_;
    wvpDate_->WVP                   = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
    wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));

    // 影行列の作成
    CreateShadowMatrix();

    Object3DPiprline::GetInstance()->PreBlendSet(DirectXCommon::GetInstance()->GetCommandList(), blendMode);

    model_->Draw(wvpResource_, shadowMap_->GetVertexResource(), material_, textureHandle);
}

///============================================================
/// 描画 (Vector3)
///============================================================

void Object3d::Draw(const Vector3& position, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    if (model_) {
      
        WorldTransform tempTransform;
        tempTransform.translation_ = position;
        tempTransform.scale_       = {1.0f, 1.0f, 1.0f}; 
        tempTransform.rotation_    = {0.0f, 0.0f, 0.0f}; 
        tempTransform.UpdateMatrix(); 

        // WVP行列の計算
        if (model_->GetIsFileGltf()) { // .gltfファイルの場合
            wvpDate_->World                 = tempTransform.matWorld_;
            wvpDate_->WVP                   = model_->GetModelData().rootNode.localMatrix * tempTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            wvpDate_->WorldInverseTranspose = Inverse(Transpose(model_->GetModelData().rootNode.localMatrix * wvpDate_->World));
        } else { // .objファイルの場合
            wvpDate_->World                 = tempTransform.matWorld_;
            wvpDate_->WVP                   = tempTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
        }

        // モデル描画
        model_->Draw(wvpResource_, shadowMap_->GetVertexResource(), material_, textureHandle);
    }
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

     // 影行列の作成
    CreateShadowMatrix();

    SkinningObject3DPipeline::GetInstance()->PreBlendSet(DirectXCommon::GetInstance()->GetCommandList(), blendMode);
    model_->DrawAnimation(wvpResource_, shadowMap_->GetVertexResource(), material_, skinCluster, textureHandle);
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

///============================================================
/// 影行列作成
///============================================================
void Object3d::CreateShadowMatrix() {
    // 平行光源の方向ベクトル
    Vector3 parallelLightVec = {Light::GetInstance()->GetDirectionalLight()->GetDirection()};

    // 地面の法線ベクトル（Y=0平面、上向き）
    Vector4 planeNormalVec = {0.0f, 1.0f, 0.0f, 0.0f};

    // 影行列を作成
    Matrix4x4 shadowMatrix = MakeShadowMatrix(planeNormalVec, parallelLightVec);

    // ShadowMapに影行列を設定
    shadowMap_->SetVertexMatrix(shadowMatrix);
}

///============================================================
/// 影行列作成関数
///============================================================
Matrix4x4 Object3d::MakeShadowMatrix(const Vector4& planeNormal, const Vector3& lightDirection) {
  
    float a = planeNormal.x;
    float b = planeNormal.y;
    float c = planeNormal.z;
    float d = planeNormal.w;

    // 光源方向ベクトル
    float lx = lightDirection.x;
    float ly = lightDirection.y;
    float lz = lightDirection.z;

    // 内積計算
    float dot = a * lx + b * ly + c * lz;

    Matrix4x4 shadowMat;

    // 影行列の計算
    shadowMat.m[0][0] = dot - a * lx;
    shadowMat.m[0][1] = -a * ly;
    shadowMat.m[0][2] = -a * lz;
    shadowMat.m[0][3] = -a * 0.0f;

    shadowMat.m[1][0] = -b * lx;
    shadowMat.m[1][1] = dot - b * ly;
    shadowMat.m[1][2] = -b * lz;
    shadowMat.m[1][3] = -b * 0.0f;

    shadowMat.m[2][0] = -c * lx;
    shadowMat.m[2][1] = -c * ly;
    shadowMat.m[2][2] = dot - c * lz;
    shadowMat.m[2][3] = -c * 0.0f;

    shadowMat.m[3][0] = -d * lx;
    shadowMat.m[3][1] = -d * ly;
    shadowMat.m[3][2] = -d * lz;
    shadowMat.m[3][3] = dot - d * 0.0f;

    return shadowMat;
}

void Object3d::CreateShadowMap() {
    shadowMap_ = std::make_unique<ShadowMap>();
    shadowMap_->Init(DirectXCommon::GetInstance());
}

///============================================================
/// マテリアルリソース作成
///============================================================
void Object3d::CreateMaterialResource() {
    BaseObject3d::CreateMaterialResource();
}