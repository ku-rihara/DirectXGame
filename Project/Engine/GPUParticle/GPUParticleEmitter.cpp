#include "GPUParticleEmitter.h"
#include "Frame/Frame.h"
#include "GPUParticleManager.h"
#include "Primitive/IPrimitive.h"

///=================================================================================
/// パーティクル作成
///=================================================================================
GPUParticleEmitter* GPUParticleEmitter::CreateParticle(
    const std::string& name,
    const std::string& modelFilePath,
    const int32_t& maxCount) {

    auto emitter = std::make_unique<GPUParticleEmitter>();
    emitter->InitWithModel(name, modelFilePath, maxCount);
    emitter->Init();
    return emitter.release();
}

///=================================================================================
/// パーティクル作成(Primitive)
///=================================================================================
GPUParticleEmitter* GPUParticleEmitter::CreateParticlePrimitive(
    const std::string& name,
    const PrimitiveType& type,
    const int32_t& maxCount) {

    auto emitter = std::make_unique<GPUParticleEmitter>();
    emitter->InitWithPrimitive(name, type, maxCount);
    emitter->Init();
    return emitter.release();
}

void GPUParticleEmitter::InitWithModel(
    const std::string& name,
    const std::string& modelFilePath,
    const int32_t& maxCount) {

    name_ = name;

    // GPUParticleManagerにパーティクルグループを作成
    GPUParticleManager::GetInstance()->CreateParticleGroup(
        name_, modelFilePath, maxCount);
}

void GPUParticleEmitter::InitWithPrimitive(
    const std::string& name,
    const PrimitiveType& type,
    const int32_t& maxCount) {

    name_ = name;

    // GPUParticleManagerにプリミティブパーティクルを作成
    GPUParticleManager::GetInstance()->CreatePrimitiveParticle(
        name_, type, maxCount);
}

///=================================================================================
/// 初期化
///=================================================================================
void GPUParticleEmitter::Init() {
    currentTime_ = 0.0f;
    frequency_   = 1.0f;
    count_       = 10;
    shouldEmit_  = false;
}

///=================================================================================
/// 更新
///=================================================================================
void GPUParticleEmitter::Update() {
    if (name_.empty()) {
        return;
    }

    // タイミング更新 
    currentTime_ += Frame::DeltaTime();

    // エミットタイミングチェック
    if (currentTime_ >= frequency_) {
        shouldEmit_  = true;
        currentTime_ = 0.0f;
    } else {
        shouldEmit_ = false;
    }

    // マネージャーのエミッターデータを更新
    auto group = GPUParticleManager::GetInstance()->GetParticleGroup(name_);
    if (group && group->emitSphereData) {
        EmitterSphere emitterData = *group->emitSphereData;
        emitterData.frequencyTime = currentTime_;
        emitterData.emit          = shouldEmit_ ? 1 : 0;

        GPUParticleManager::GetInstance()->SetEmitterSphere(name_, emitterData);
    }
}

///=================================================================================
/// エミット
///=================================================================================
void GPUParticleEmitter::Emit() {
    if (name_.empty()) {
        return;
    }

    // 即座にエミットをトリガー
    GPUParticleManager::GetInstance()->Emit(name_);
}



void GPUParticleEmitter::SetEmitterData(const EmitterSphere& emitter) {
    if (name_.empty()) {
        return;
    }

    frequency_ = emitter.frequency;
    count_     = emitter.count;

    GPUParticleManager::GetInstance()->SetEmitterSphere(name_, emitter);
}

///=================================================================================
/// テクスチャ切り替え
///=================================================================================
void GPUParticleEmitter::SetTexture(uint32_t textureHandle) {
    if (name_.empty()) {
        return;
    }

    GPUParticleManager::GetInstance()->SetTextureHandle(name_, textureHandle);
}