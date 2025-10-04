#pragma once
#include "3d/ViewProjection.h"
#include "base/SrvManager.h"
#include "Data/ParticleCSData.h"
#include "Dx/DirectXCommon.h"
#include "GPUParticleEmitter.h"
#include "Material/ParticleMaterial.h"
#include "ResourceCreator/GPUParticleResourceCreator.h"
#include "Primitive/IPrimitive.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

class Model;
class IPrimitive;
class ViewProjection;

class GPUParticleManager {
public:
    struct GPUParticleGroup {
        Model* model                           = nullptr;
        std::unique_ptr<IPrimitive> primitive_ = nullptr;
        ParticleMaterial material;
        uint32_t textureHandle   = 0;
        int32_t maxParticleCount = 1024;

        // GPU Resources
        std::unique_ptr<GPUParticleResourceCreator> resourceCreator;
        std::unique_ptr<GPUParticleEmitter> emitter;

        // Mapped data pointers
        EmitterSphere* emitSphereData = nullptr;
        PerView* perViewData          = nullptr;
    };

public:
    GPUParticleManager()  = default;
    ~GPUParticleManager() = default;

    static GPUParticleManager* GetInstance();

    // 初期化
    void Init(SrvManager* srvManager);

    // 更新・描画
    void Update();
    void Draw(const ViewProjection& viewProjection);

    // グループ作成
    void CreateParticleGroup(
        const std::string& name,
        const std::string& modelFilePath,
        const int32_t& maxCount = 1024);

    void CreatePrimitiveParticle(
        const std::string& name,
        PrimitiveType type,
        const int32_t& maxCount);

private:
    void InitializeGroupResources(GPUParticleGroup& group);
    void DispatchComputeShaders(GPUParticleGroup& group);
    void CreateMaterialResource(const std::string& name);
    void DrawGroup(GPUParticleGroup& group);
    void DispatchInitParticle(GPUParticleGroup& group);

private:
    DirectXCommon* dxCommon_              = nullptr;
    SrvManager* srvManager_               = nullptr;
    const ViewProjection* viewProjection_ = nullptr;

    // Particle Groups
    std::unordered_map<std::string, GPUParticleGroup> particleGroups_;

public:
    // Getter
    GPUParticleGroup* GetParticleGroup(const std::string& name);

    // setter
    void SetTextureHandle(const std::string& name, const uint32_t& handle);
    void SetModel(const std::string& name, const std::string& modelName);
    void SetEmitterSphere(const std::string& name, const EmitterSphere& emitter);
    void SetViewProjection(const ViewProjection* view);
};