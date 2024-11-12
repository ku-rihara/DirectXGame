#include"Object3d.h"
#include"ModelManager.h"

#include<cmath>
#include<numbers>

Object3d::Object3d() {
}

Object3d::~Object3d() {
}

Object3d* Object3d::CreateModel(const std::string& instanceName, const std::string& extension) {
    // 新しいModelインスタンスを作成
    Object3d* object3d = new Object3d();
    ModelManager::GetInstance()->LoadModel(instanceName, extension);
    object3d->SetModel(instanceName, extension);
    object3d->CreateWVPResource();
    object3d->CreateMaterialResource();
    return object3d;
}

/// 初期化
void Object3d::Init() {
    // WVPリソース作成
    CreateWVPResource();
}

// 更新
void Object3d::Update() {
    color_.TransferMatrix();
    material_.materialData_->color = color_.GetColor();
}

// 描画
void Object3d::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    if (model_) {
        // WVP行列の計算
        if (model_->GetIsFileGltf()) { //.gltfファイルの場合
            wvpDate_->WVP = model_->GetModelData().rootNode.localMatrix * worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            wvpDate_->WorldInverseTranspose = Inverse(Transpose(model_->GetModelData().rootNode.localMatrix * wvpDate_->World));
        }
        else { //.objファイルの場合
            wvpDate_->WVP = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));
        }

        // オブジェクトのビュー座標系での位置
        Vector3 positionView = MatrixTransform(worldTransform.GetWolrdPosition(), viewProjection.matView_);

        if (!worldTransform.isNotViewRestriction_) {
            // 距離条件チェック
            if (0 <= positionView.z && positionView.z <= WinApp::kWindowWidth) {

                // 視野角条件チェック
                float viewAngleLimit = 45.0f * std::numbers::pi_v<float> / 180.0f; // 45度の視野角

                // X軸方向とY軸方向のスケール補正視野角判定
                float actAngleX = std::atan2(positionView.x + worldTransform.GetScale().x, positionView.z);
                float actAngleY = std::atan2(positionView.y + worldTransform.GetScale().y, positionView.z);

                // 角度条件（X軸とY軸両方の視野範囲に収まっているか）
                if (std::fabsf(actAngleX) <= viewAngleLimit && std::fabsf(actAngleY) <= viewAngleLimit) {
                    model_->Draw(wvpResource_, material_, textureHandle);
                }

            }
        }
        else {       
                model_->Draw(wvpResource_, material_, textureHandle);
            
        }
    }
}

void Object3d::DebugImgui() {
    BaseObject3d::DebugImgui();
}

void Object3d::CreateWVPResource() {
    wvpResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
    // データを書き込む
    wvpDate_ = nullptr;
    // 書き込むためのアドレスを取得
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
    // 単位行列を書き込んでおく
    wvpDate_->WVP = MakeIdentity4x4();
    wvpDate_->World = MakeIdentity4x4();
    wvpDate_->WorldInverseTranspose = MakeIdentity4x4();
}

void Object3d::CreateMaterialResource() {
    BaseObject3d::CreateMaterialResource();
}
