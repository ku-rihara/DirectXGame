#include "ParticleGroupRegistry.h"

using namespace KetaEngine;

#include "Base/Dx/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "MathFunction.h"
// Primitive
#include "3d/ModelManager.h"
#include "3D/Primitive/PrimitiveBox.h"
#include "3D/Primitive/PrimitiveCylinder.h"
#include "3D/Primitive/PrimitivePlane.h"
#include "3D/Primitive/PrimitiveRing.h"
#include "3D/Primitive/PrimitiveSphere.h"

void ParticleGroupRegistry::SetModel(
    const std::string& modelName,
    ParticleManager::ParticleGroup& group) {

    // モデルを検索してセット
    ModelManager::GetInstance()->LoadModel(modelName);
    group.model         = ModelManager::GetInstance()->FindModel(modelName);
    group.textureHandle = TextureManager::GetInstance()->LoadTexture(
        group.model->GetModelData().material.textureFilePath);
}

void ParticleGroupRegistry::CreateMaterialResource(ParticleManager::ParticleGroup& group) {
    group.material.Init(DirectXCommon::GetInstance());
}

void ParticleGroupRegistry::ResetInstancingData(
    const std::string& name,
    ParticleManager::ParticleGroup& group) {

    for (uint32_t index = 0; index < group.instanceNum; ++index) {
        group.instancingData[index].WVP                   = MakeIdentity4x4();
        group.instancingData[index].World                 = MakeIdentity4x4();
        group.instancingData[index].WorldInverseTranspose = MakeIdentity4x4();
        group.instancingData[index].UVTransform           = MakeIdentity4x4();
        group.instancingData[index].color                 = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        group.instancingData[index].isFlipX               = 0u;
        group.instancingData[index].isFlipY               = 0u;
        group.instancingData[index].dissolveThreshold     = 1.0f;
    }

    (void)name;
}

void ParticleGroupRegistry::CreateInstancingResource(
    const std::string& name,
    uint32_t instanceNum,
    ParticleManager::ParticleGroup& group,
    SrvManager* srvManager) {

    group.instanceNum = instanceNum;
    group.currentNum  = instanceNum;

    // Instancing用のTransformationMatrixリソースを作る
    group.instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
        DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * instanceNum);

    if (!group.instancingResource) {
        return;
    }

    group.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&group.instancingData));

    // インスタンシングデータリセット
    ResetInstancingData(name, group);

    // SRV確保
    group.srvIndex = srvManager->Allocate();

    // SRVの作成
    srvManager->CreateForStructuredBuffer(
        group.srvIndex,
        group.instancingResource.Get(),
        instanceNum,
        sizeof(ParticleFprGPU));
}

void ParticleGroupRegistry::CreateParticleGroup(
    const std::string& name,
    const std::string& modelFilePath,
    uint32_t maxnum,
    std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
    SrvManager* srvManager) {

    if (groups.contains(name)) {
        return;
    }

    // グループ追加
    groups[name] = ParticleManager::ParticleGroup();

    /// モデル
    ModelManager::GetInstance()->LoadModel(modelFilePath);
    SetModel(modelFilePath, groups[name]);

    /// リソース作成
    CreateInstancingResource(name, maxnum, groups[name], srvManager); // インスタンシング
    CreateMaterialResource(groups[name]); // マテリアル

    groups[name].instanceNum = 0;
}

void ParticleGroupRegistry::CreatePrimitiveParticle(
    const std::string& name,
    PrimitiveType type,
    uint32_t maxnum,
    std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
    SrvManager* srvManager) {

    if (groups.contains(name)) {
        return;
    }

    // グループを追加
    groups[name] = ParticleManager::ParticleGroup();

    // createPrimitive
    switch (type) {
    case PrimitiveType::Plane:
        groups[name].primitive_ = std::make_unique<PrimitivePlane>();
        break;
    case PrimitiveType::Sphere:
        groups[name].primitive_ = std::make_unique<PrimitiveSphere>();
        break;
    case PrimitiveType::Box:
        groups[name].primitive_ = std::make_unique<PrimitiveBox>();
        break;
    case PrimitiveType::Ring:
        groups[name].primitive_ = std::make_unique<PrimitiveRing>();
        break;
    case PrimitiveType::Cylinder:
        groups[name].primitive_ = std::make_unique<PrimitiveCylinder>();
        break;
    }

    // プリミティブの初期化と作成
    groups[name].primitive_->Init();

    // インスタンシングリソースとマテリアルリソースを作成
    CreateInstancingResource(name, maxnum, groups[name], srvManager);
    CreateMaterialResource(groups[name]);

    groups[name].instanceNum = 0;
}

void ParticleGroupRegistry::ResetAllParticles(
    std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups) {

    for (auto& groupPair : groups) {
        ParticleManager::ParticleGroup& group = groupPair.second;

        // 全てのパーティクルを消去
        group.particles.clear();

        // インスタンシングデータをリセット
        for (uint32_t index = 0; index < group.instanceNum; ++index) {
            group.instancingData[index].WVP                   = MakeIdentity4x4();
            group.instancingData[index].World                 = MakeIdentity4x4();
            group.instancingData[index].WorldInverseTranspose = MakeIdentity4x4();
            group.instancingData[index].UVTransform           = MakeIdentity4x4();
            group.instancingData[index].color                 = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
            group.instancingData[index].isFlipX               = 0u;
            group.instancingData[index].isFlipY               = 0u;
            group.instancingData[index].dissolveThreshold     = 1.0f;
        }
    }
}
