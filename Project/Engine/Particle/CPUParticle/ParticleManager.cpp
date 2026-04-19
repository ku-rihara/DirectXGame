#include "ParticleManager.h"

using namespace KetaEngine;

// サブクラス
#include "SubSystems/Factory/ParticleFactory.h"
#include "SubSystems/Updater/ParticleUpdater.h"
#include "SubSystems/Renderer/ParticleRenderer.h"
#include "SubSystems/Registry/ParticleGroupRegistry.h"

// Base
#include "Base/TextureManager.h"
#include "3d/ModelManager.h"
// frame
#include "Frame/Frame.h"
// Function
#include "Function/GetFile.h"
// editor
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cassert>
#include <string>

ParticleManager* ParticleManager::GetInstance() {
    static ParticleManager instance;
    return &instance;
}

///============================================================
/// 初期化
///============================================================
void ParticleManager::Init(SrvManager* srvManager) {
    pSrvManager_ = srvManager;

    factory_  = std::make_unique<ParticleFactory>();
    updater_  = std::make_unique<ParticleUpdater>();
    renderer_ = std::make_unique<ParticleRenderer>();
    registry_ = std::make_unique<ParticleGroupRegistry>();

    SetAllParticleFile();
}

///============================================================
/// 更新
///============================================================
void ParticleManager::Update() {
    for (auto& groupPair : particleGroups_) {
        updater_->UpdateGroup(groupPair.second, accelerationField_, viewProjection_);
    }
}

///============================================================
/// 描画
///============================================================
void ParticleManager::Draw(const ViewProjection& viewProjection) {
    renderer_->Draw(particleGroups_, viewProjection, pSrvManager_);
}

///============================================================
/// スクリーン座標系パーティクル描画
///============================================================
void ParticleManager::DrawScreenPos() {
    renderer_->DrawScreenPos(particleGroups_, pSrvManager_);
}

///============================================================
/// 歪みパスパーティクル描画
///============================================================
void ParticleManager::DrawDistortion(const ViewProjection& viewProjection) {
    renderer_->DrawDistortion(particleGroups_, viewProjection, pSrvManager_);
}

void ParticleManager::UpdateUV(UVInfo& uvInfo, float deltaTime) {
    updater_->UpdateUV(uvInfo, deltaTime);
}

///============================================================
/// グループ作成
///============================================================
void ParticleManager::CreateParticleGroup(const std::string name, const std::string modelFilePath, uint32_t maxnum) {
    registry_->CreateParticleGroup(name, modelFilePath, maxnum, particleGroups_, pSrvManager_);
}

void ParticleManager::CreatePrimitiveParticle(const std::string& name, PrimitiveType type, uint32_t maxnum) {
    registry_->CreatePrimitiveParticle(name, type, maxnum, particleGroups_, pSrvManager_);
}

///============================================================
/// テクスチャセット
///============================================================
void ParticleManager::SetTextureHandle(const std::string name, uint32_t handle) {
    particleGroups_[name].textureHandle = handle;
}

void ParticleManager::SetDissolveTextureHandle(const std::string& name, uint32_t handle) {
    particleGroups_[name].dissolveTextureHandle = handle;
}

void ParticleManager::SetDistortionTextureHandle(const std::string& name, uint32_t handle) {
    particleGroups_[name].distortionTextureHandle = handle;
}

void ParticleManager::PlayDissolve(const std::string& name, const std::string& dissolveName) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }
    auto& group = it->second;
    if (!group.dissolvePlayer) {
        group.dissolvePlayer = std::make_unique<DissolvePlayer>();
    }
    group.dissolvePlayer->Play(dissolveName);
    group.lastDissolveTexturePath.clear();
}

void ParticleManager::StopDissolve(const std::string& name) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }
    auto& group = it->second;
    if (group.dissolvePlayer) {
        group.dissolvePlayer->Stop();
    }
    group.material.GetMaterialData()->enableDissolve = 0;
    group.dissolveTextureHandle                      = 0;
    group.lastDissolveTexturePath.clear();
    group.dissolveParams                             = DissolveGroupParams{};
}

///============================================================
/// モデルセット
///============================================================
void ParticleManager::SetModel(const std::string& name, const std::string& modelName) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }
    ModelManager* mm = ModelManager::GetInstance();
    mm->LoadModel(modelName);
    it->second.model         = mm->FindModel(modelName);
    it->second.textureHandle = TextureManager::GetInstance()->LoadTexture(
        it->second.model->GetModelData().material.textureFilePath);
}

void ParticleManager::CreateMaterialResource(const std::string& name) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }
    it->second.material.Init(DirectXCommon::GetInstance());
}

void ParticleManager::CreateInstancingResource(const std::string& name, uint32_t instanceNum) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }
    // registryの内部メソッドと同等の処理を直接実行
    ParticleGroup& group = it->second;

    group.instanceNum = instanceNum;
    group.currentNum  = instanceNum;

    group.instancingResource = DirectXCommon::GetInstance()->CreateBufferResource(
        DirectXCommon::GetInstance()->GetDevice(), sizeof(ParticleFprGPU) * instanceNum);

    if (!group.instancingResource) {
        return;
    }

    group.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&group.instancingData));

    ResetInstancingData(name);

    group.srvIndex = pSrvManager_->Allocate();

    pSrvManager_->CreateForStructuredBuffer(
        group.srvIndex,
        group.instancingResource.Get(),
        instanceNum,
        sizeof(ParticleFprGPU));
}

///======================================================================
/// パーティクル作成
///======================================================================
ParticleManager::Particle ParticleManager::MakeParticle(const Parameters& parameters, const DissolveGroupParams* dissolveParams) {
    return factory_->MakeParticle(parameters, dissolveParams, viewProjection_);
}

///======================================================================
/// エミット
///======================================================================
void ParticleManager::Emit(
    std::string name, const Parameters& parameters, const GroupParameters& groupParameters, int32_t count) {

    // パーティクルグループが存在するか確認
    assert(particleGroups_.find(name) != particleGroups_.end() && "Error: Not Find ParticleGroup");
   
    // 指定されたパーティクルグループを取得
    ParticleGroup& particleGroup = particleGroups_[name];
    particleGroup.param          = groupParameters;

    // 生成、グループ追加
    std::list<Particle> particles;
    for (uint32_t i = 0; i < uint32_t(count); ++i) {
        Particle p = MakeParticle(parameters, &particleGroup.dissolveParams);
        if (groupParameters.isScreenPos) {
            if (p.isFloatVelocity) {
                p.speed_ *= Frame::GetFPS();
            } else {
                p.speedV3 *= Frame::GetFPS();
            }
        }
        particles.emplace_back(std::move(p));
    }

    // グループに追加
    particleGroup.particles.splice(particleGroup.particles.end(), particles);
}

/// Reset
void ParticleManager::ResetAllParticles() {
    registry_->ResetAllParticles(particleGroups_);
}

void ParticleManager::ResetInstancingData(const std::string& name) {
    auto it = particleGroups_.find(name);
    if (it == particleGroups_.end()) {
        return;
    }
    registry_->ResetInstancingData(name, it->second);
}

///=================================================================================================
/// param Adapt
///=================================================================================================
void ParticleManager::AlphaAdapt(ParticleFprGPU& data, const Particle& parm) {
    data.color = parm.color_;
    switch (parm.alphaMode) {
    case ParticleCommon::AlphaMode::None:
        data.color.w = 1.0f;
        break;
    case ParticleCommon::AlphaMode::Easing:
        break;
    case ParticleCommon::AlphaMode::LifeTime:
        [[fallthrough]];
    default:
        data.color.w = 1.0f - (parm.currentTime_ / parm.lifeTime_);
        break;
    }
}

bool ParticleManager::HasDistortionParticles() const {
    for (const auto& pair : particleGroups_) {
        const ParticleGroup& group = pair.second;
        if (group.param.useDistortion && group.distortionTextureHandle != 0 && !group.param.isScreenPos) {
            return true;
        }
    }
    return false;
}

void ParticleManager::SetViewProjection(const ViewProjection* view) {
    viewProjection_ = view;
}

void ParticleManager::SetAllParticleFile() {
    particleFiles_ = GetFileNamesForDirectory(GlobalParameter::GetInstance()->GetDirectoryPath() + ParticleFolderName_);
}
