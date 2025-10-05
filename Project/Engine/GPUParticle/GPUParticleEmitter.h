#pragma once
#include "Data/GPUParticleEmitterData.h"
#include "Vector3.h"
#include <memory>
#include <string>

class GPUParticleManager;
enum class PrimitiveType;

class GPUParticleEmitter {
public:
    GPUParticleEmitter()  = default;
    ~GPUParticleEmitter() = default;

  
    static GPUParticleEmitter* CreateParticle(const std::string& name,const std::string& modelFilePath,const int32_t& maxCount = 1024);
    static GPUParticleEmitter* CreateParticlePrimitive(const std::string& name,const PrimitiveType& type,const int32_t& maxCount = 1024);

    // Init
    void Init();

    // Update,Emit
    void Update();
    void Emit();

    void SetEmitterData(const EmitterSphere& emitter);
    void SetTexture(uint32_t textureHandle);


private:
    void InitWithModel(const std::string& name,const std::string& modelFilePath, const int32_t& maxCount);

    void InitWithPrimitive(const std::string& name,const PrimitiveType& type,const int32_t& maxCount);

private:
    std::string name_;

    // Emitter state
    float currentTime_ = 0.0f;
    float frequency_   = 1.0f;
    uint32_t count_    = 10;
    bool shouldEmit_   = false;

public:
    // Getters
    const std::string& GetName() const { return name_; }
    const bool& IsValid() const { return !name_.empty(); }
   const float& GetFrequencyTime() const { return currentTime_; }
    uint32_t GetEmitFlag() const { return shouldEmit_ ? 1 : 0; }
};