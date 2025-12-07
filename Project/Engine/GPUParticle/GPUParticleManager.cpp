#include "GPUParticleManager.h"
#include "3d/ModelManager.h"
#include "3d/ViewProjection.h"
#include "base/TextureManager.h"
// dx
#include "Dx/DxResourceBarrier.h"
#include "Frame/Frame.h"
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
    int32_t maxCount) {

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
    int32_t maxCount) {

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

    group.textureHandle = TextureManager::GetInstance()->LoadTexture("Resources/Texture/circle.png");

    // リソース作成
    InitializeGroupResources(group);

    // マテリアル作成
    CreateMaterialResource(name);
}

void GPUParticleManager::InitializeGroupResources(GPUParticleGroup& group) {
    // リソースクリエイター作成
    group.resourceData = std::make_unique<GPUParticleResourceData>();
    group.resourceData->SetParticleMaxCount(group.maxParticleCount);
    group.resourceData->Create();

    DispatchInitParticle(group);

    // マップされたデータポインタを取得
    group.emitSphereData = group.resourceData->GetEmitterBuffer().mappedData;
    group.perViewData    = group.resourceData->GetPerViewBuffer().mappedData;

    // デフォルト値を設定
    if (group.emitSphereData) {
        group.emitSphereData->translate     = Vector3(0.0f, 0.0f, 0.0f);
        group.emitSphereData->count         = 10;
        group.emitSphereData->frequency     = 1.0f;
        group.emitSphereData->frequencyTime = 0.0f;
        group.emitSphereData->emit          = 0;
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

        // Particle更新
        // TODO:DeltaTimeはParticleごとに設定できるようにする
        group.resourceData->UpdatePerFrameData(Frame::DeltaTime());
        DispatchEmit(group);
        DispatchUpdate(group);
    }
}

void GPUParticleManager::DispatchInitParticle(GPUParticleGroup& group) {
    if (!group.resourceData) {
        return;
    }

    // Initのシェーダーバインド、ディスパッチを行う
    group.resourceData->GetCommandExecutorRef()->ExecuteInitPass(dxCommon_->GetCommandList(), group.maxParticleCount);
}

void GPUParticleManager::DispatchEmit(GPUParticleGroup& group) {
    if (!group.resourceData) {
        return;
    }

    // Emitのシェーダーバインド、ディスパッチを行う
    group.resourceData->GetCommandExecutorRef()->ExecuteEmitPass(dxCommon_->GetCommandList(), 1);
}

void GPUParticleManager::DispatchUpdate(GPUParticleGroup& group) {
    if (!group.resourceData) {
        return;
    }
    // Initのシェーダーバインド、ディスパッチを行う
    group.resourceData->GetCommandExecutorRef()->ExecuteUpdatePass(dxCommon_->GetCommandList(), group.maxParticleCount);
}

void GPUParticleManager::Draw(const ViewProjection& viewProjection) {
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
    PipelineManager* pipe                  = PipelineManager::GetInstance();
    viewProjection;

    for (auto& [groupName, group] : particleGroups_) {
        if (!group.resourceData) {
            continue;
        }

        // レンダリングパイプライン設定
        pipe->PreDraw(PipelineType::GPUParticle, commandList);
        pipe->PreBlendSet(PipelineType::GPUParticle, commandList, BlendMode::Add);

        // b0: PerViewバッファ
        commandList->SetGraphicsRootConstantBufferView(0, group.resourceData->GetPerViewBuffer().resource->GetGPUVirtualAddress());

        // t0: パーティクルSRV
        commandList->SetGraphicsRootDescriptorTable(1, group.resourceData->GetParticleBuffer().gpuHandle);

        // b2(PS): マテリアル
        group.material.SetCommandList(commandList);

        // t0(PS): テクスチャ
        commandList->SetGraphicsRootDescriptorTable(3, TextureManager::GetInstance()->GetTextureHandle(group.textureHandle));

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

void GPUParticleManager::SetModel(const std::string& name, const std::string& modelName) {
    assert(particleGroups_.contains(name));

    ModelManager::GetInstance()->LoadModel(modelName);
    particleGroups_[name].model = ModelManager::GetInstance()->FindModel(modelName);

    if (particleGroups_[name].model) {
        particleGroups_[name].textureHandle = TextureManager::GetInstance()->LoadTexture(
            particleGroups_[name].model->GetModelData().material.textureFilePath);
    }
}

void GPUParticleManager::SetTextureHandle(const std::string& name, uint32_t handle) {
    assert(particleGroups_.contains(name));
    particleGroups_[name].textureHandle = handle;
}

void GPUParticleManager::CreateMaterialResource(const std::string& name) {
    assert(particleGroups_.contains(name));
    particleGroups_[name].material.CreateMaterialResource(dxCommon_);
}

void GPUParticleManager::SetEmitterSphere(const std::string& name, const ParticleEmit& emitter) {
    assert(particleGroups_.contains(name));

    GPUParticleGroup& group = particleGroups_[name];
    if (group.emitSphereData) {
        *group.emitSphereData = emitter;
    }
}