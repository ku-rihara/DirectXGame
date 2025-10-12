#include "GPUParticleManager.h"
#include "3d/ModelManager.h"
#include "3d/ViewProjection.h"
#include "base/TextureManager.h"
// pipeline
#include "Pipeline/CSPipelineManager.h"
#include "Pipeline/PipelineManager.h"
// primitive
#include "Primitive/PrimitiveCylinder.h"
#include "Primitive/PrimitivePlane.h"
#include "Primitive/PrimitiveRing.h"
#include <cassert>

GPUParticleManager* GPUParticleManager::GetInstance() {
    static GPUParticleManager instance;
    return &instance;
}

void GPUParticleManager::Init(SrvManager* srvManager) {
    dxCommon_   = DirectXCommon::GetInstance();
    srvManager_ = srvManager;
}

void GPUParticleManager::CreateParticleGroup(
    const std::string& name,
    const std::string& modelFilePath,
    const int32_t& maxCount) {

    if (particleGroups_.contains(name)) {
        return;
    }

    particleGroups_[name]   = GPUParticleGroup();
    GPUParticleGroup& group = particleGroups_[name];
    group.maxParticleCount  = maxCount;

    // モデル読み込み
    ModelManager::GetInstance()->LoadModel(modelFilePath);
    SetModel(name, modelFilePath);

    // リソース作成
    InitializeGroupResources(group);

    // マテリアル作成
    CreateMaterialResource(name);
}

void GPUParticleManager::CreatePrimitiveParticle(
    const std::string& name,
    const PrimitiveType& type,
    const int32_t& maxCount) {

    if (particleGroups_.contains(name)) {
        return;
    }

    particleGroups_[name]   = GPUParticleGroup();
    GPUParticleGroup& group = particleGroups_[name];
    group.maxParticleCount  = maxCount;

    // プリミティブ作成
    switch (type) {
    case PrimitiveType::Plane:
        group.primitive_ = std::make_unique<PrimitivePlane>();
        break;
    case PrimitiveType::Ring:
        group.primitive_ = std::make_unique<PrimitiveRing>();
        break;
    case PrimitiveType::Cylinder:
        group.primitive_ = std::make_unique<PrimitiveCylinder>();
        break;
    }

    if (group.primitive_) {
        group.primitive_->Init();
        group.primitive_->Create();
    }

    // リソース作成
    InitializeGroupResources(group);

    // マテリアル作成
    CreateMaterialResource(name);
}

void GPUParticleManager::DispatchInitParticle(GPUParticleGroup& group) {
    if (!group.resourceCreator) {
        return;
    }

    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
    CSPipelineManager* csPipe              = CSPipelineManager::GetInstance();

    //  デスクリプタヒープを設定
    ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager_->GetDescriptorHeap()};
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // InitParticleパス
    csPipe->PreDraw(CSPipelineType::Particle_Init, commandList);

    // u0: パーティクルバッファ(UAV)
    commandList->SetComputeRootDescriptorTable(0,
        group.resourceCreator->GetParticleUavHandle());

    csPipe->DisPatch(CSPipelineType::Particle_Init, commandList, group.maxParticleCount);

    // UAVバリア
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource          = group.resourceCreator->GetParticleResource();
    commandList->ResourceBarrier(1, &barrier);
}

void GPUParticleManager::InitializeGroupResources(GPUParticleGroup& group) {
    // リソースクリエイター作成
    group.resourceCreator = std::make_unique<GPUParticleResourceCreator>();
    group.resourceCreator->SetParticleCountMax(group.maxParticleCount);
    group.resourceCreator->Create();

    DispatchInitParticle(group);

    // マップされたデータポインタを取得
    group.emitSphereData = group.resourceCreator->GetEmitterData();
    group.perViewData    = group.resourceCreator->GetPerViewData();

    // デフォルト値を設定
    if (group.emitSphereData) {
        group.emitSphereData->translate     = Vector3(0.0f, 0.0f, 0.0f);
        group.emitSphereData->radius        = 1.0f;
        group.emitSphereData->count         = 10;
        group.emitSphereData->frequency     = 1.0f;
        group.emitSphereData->frequencyTime = 0.0f;
        group.emitSphereData->emit          = 0;
    }

}

void GPUParticleManager::SetModel(const std::string& name, const std::string& modelName) {
    assert(particleGroups_.contains(name));

    ModelManager::GetInstance()->LoadModel(modelName);
    particleGroups_[name].model = ModelManager::GetInstance()->FindModel(modelName);

    if (particleGroups_[name].model) {
        particleGroups_[name].textureHandle = TextureManager::GetInstance()->LoadTexture(
            particleGroups_[name].model->GetModelData().material.textureFilePath);
    }
}

void GPUParticleManager::SetTextureHandle(const std::string& name, const uint32_t& handle) {
    assert(particleGroups_.contains(name));
    particleGroups_[name].textureHandle = handle;
}

void GPUParticleManager::CreateMaterialResource(const std::string& name) {
    assert(particleGroups_.contains(name));
    particleGroups_[name].material.CreateMaterialResource(dxCommon_);
}

void GPUParticleManager::SetEmitterSphere(const std::string& name, const EmitterSphere& emitter) {
    assert(particleGroups_.contains(name));

    GPUParticleGroup& group = particleGroups_[name];
    if (group.emitSphereData) {
        *group.emitSphereData = emitter;
    }
}

void GPUParticleManager::Emit(const std::string& name) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }

    GPUParticleGroup& group = it->second;

    if (!group.emitSphereData) {
        return;
    }

    // 即座にエミット
    group.emitSphereData->emit = 1;
}

void GPUParticleManager::Update() {
    for (auto& [groupName, group] : particleGroups_) {

        // ViewProjectionデータ更新
        if (viewProjection_ && group.perViewData) {
            group.perViewData->viewProjection =
                viewProjection_->matView_ * viewProjection_->matProjection_;
            group.perViewData->billboardMatrix =
                viewProjection_->GetBillboardMatrix();
        }

        // コンピュートシェーダーディスパッチ
        DispatchComputeShaders(group);
    }
}

void GPUParticleManager::DispatchComputeShaders(GPUParticleGroup& group) {
    if (!group.resourceCreator) {
        return;
    }

    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
    CSPipelineManager* csPipe              = CSPipelineManager::GetInstance();

    //  デスクリプタヒープを設定
    ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager_->GetDescriptorHeap()};
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // エミットパス
    csPipe->PreDraw(CSPipelineType::Particle_Emit, commandList);

    // b0: エミッターデータ
    commandList->SetComputeRootConstantBufferView(0,
        group.resourceCreator->GetEmitterResource()->GetGPUVirtualAddress());

    // u0: パーティクルバッファ(UAV)
    commandList->SetComputeRootDescriptorTable(1,
        group.resourceCreator->GetParticleUavHandle());

    // 1スレッドでエミット処理
    csPipe->DisPatch(CSPipelineType::Particle_Emit, commandList, 1);

    // UAV→SRVバリア
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_UAV;
    barrier.UAV.pResource          = group.resourceCreator->GetParticleResource();
    commandList->ResourceBarrier(1, &barrier);
}
void GPUParticleManager::Draw(const ViewProjection& viewProjection) {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
    PipelineManager* pipe                  = PipelineManager::GetInstance();
    viewProjection;

    for (auto& [groupName, group] : particleGroups_) {
        if (!group.resourceCreator) {
            continue;
        }

        // レンダリングパイプライン設定
        pipe->PreDraw(PipelineType::GPUParticle, commandList);
        pipe->PreBlendSet(PipelineType::GPUParticle, commandList, BlendMode::Add);

        // b0: PerViewバッファ
        commandList->SetGraphicsRootConstantBufferView(0,
            group.resourceCreator->GetPerViewResource()->GetGPUVirtualAddress());

        // t0: パーティクルSRV
        commandList->SetGraphicsRootDescriptorTable(1,
            group.resourceCreator->GetParticleSrvHandle());

        // b2(PS): マテリアル
        group.material.SetCommandList(commandList);

        // t0(PS): テクスチャ
        commandList->SetGraphicsRootDescriptorTable(3,
            srvManager_->GetGPUDescriptorHandle(group.textureHandle));

        // 描画
        DrawGroup(group);
    }
}

void GPUParticleManager::DrawGroup(GPUParticleGroup& group) {
    if (group.model) {
        // モデル描画
        group.model->DrawInstancing(group.maxParticleCount);

    } else if (group.primitive_ && group.primitive_->GetMesh()) {
        // プリミティブ描画
        group.primitive_->GetMesh()->DrawInstancing(group.maxParticleCount);
    }
}

void GPUParticleManager::SetViewProjection(const ViewProjection* view) {
    viewProjection_ = view;
}

GPUParticleManager::GPUParticleGroup* GPUParticleManager::GetParticleGroup(const std::string& name) {
    auto it = particleGroups_.find(name);
    if (it != particleGroups_.end()) {
        return &it->second;
    }
    return nullptr;
}