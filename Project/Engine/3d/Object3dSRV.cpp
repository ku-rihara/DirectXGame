#include "Object3dSRV.h"
#include "base/DirectXCommon.h"
#include "base/SrvManager.h"
#include "ModelManager.h"

Object3dSRV* Object3dSRV::CreateModel(
    const std::string& instanceName, const std::string& extension,
    const uint32_t& instanceNum, SrvManager* srvManager) {
    // インスタンスの生成
    Object3dSRV* instance  = new Object3dSRV();
    instance->pSrvManager_ = srvManager;
    instance->instanceMax_ = instanceNum;

    // 必要な初期化処理を追加
    ModelManager::GetInstance()->LoadModel(instanceName, extension);
    instance->SetModel(instanceName, extension);
    instance->CreateInstancingResource(instanceNum);
    instance->CreateMaterialResource();
    instance->Clear();

    return instance;
}

void Object3dSRV::UpdateTransform(const BillboardType& axis, std::optional<const ViewProjection*> viewProjection) {
    for (std::list<WorldTransform>::iterator particleIterator = particles_.begin();
        particleIterator != particles_.end(); ++particleIterator) {

        if (viewProjection.has_value()) { // ビルボード
            (*particleIterator).BillboardUpdateMatrix(*viewProjection.value(), axis);
        } else { // 普通のUpdateMatirx
            (*particleIterator).UpdateMatrix();
        }
    }
}

void Object3dSRV::Draw(const std::list<WorldTransform>& transforms, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    instanceNum_ = 0; // 描画するインスタンス数をリセット

    auto it = transforms.begin(); // 渡されたリストを参照
    for (; it != transforms.end() && instanceNum_ < instanceMax_; ++it) {
        // WVP行列の計算
        if (model_->GetIsFileGltf()) {
            instancingData_[instanceNum_].WVP                   = model_->GetModelData().rootNode.localMatrix * it->matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            instancingData_[instanceNum_].WorldInverseTranspose = Inverse(Transpose(model_->GetModelData().rootNode.localMatrix * it->matWorld_));
        } else {
            instancingData_[instanceNum_].WVP                   = it->matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
            instancingData_[instanceNum_].WorldInverseTranspose = Inverse(Transpose(it->matWorld_));
        }

        ++instanceNum_;
    }

    // 描画処理
    if (model_) {
        model_->DrawInstancing(instanceNum_, pSrvManager_->GetGPUDescriptorHandle(srvIndex_), material_, textureHandle);
    }
}

void Object3dSRV::DebugImgui() {
    BaseObject3d::DebugImgui();
}

void Object3dSRV::CreateInstancingResource(const uint32_t& instanceNum) {
    instanceNum_ = instanceNum;

    // Instancing用のTransformationMatrixリソースを作る
    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
        DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix) * instanceNum_);

    instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));

    for (uint32_t index = 0; index < instanceNum_; ++index) {
        instancingData_[index].WVP   = MakeIdentity4x4();
        instancingData_[index].World = MakeIdentity4x4();
    }

    // SRV確保
    srvIndex_ = pSrvManager_->Allocate();

    // SRVの作成
    pSrvManager_->CreateSRVforStructuredBuffer(
        srvIndex_,
        instancingResource.Get(),
        instanceNum,
        sizeof(TransformationMatrix));
}

void Object3dSRV::Clear() {

    particles_.clear(); // リストをクリア
}

void Object3dSRV::CreateMaterialResource() {
    BaseObject3d::CreateMaterialResource();
}