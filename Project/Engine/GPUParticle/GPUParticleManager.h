#pragma once
#include "3d/ViewProjection.h"
#include "base/SrvManager.h"
#include "Data/ParticleCSData.h"
#include "Dx/DirectXCommon.h"
#include "Material/GPUParticleMaterial.h"
#include "Primitive/IPrimitive.h"
#include "ResourceCreator/GPUParticleResourceCreator.h"

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
        GPUParticleMaterial material;
        uint32_t textureHandle   = 0;
        int32_t maxParticleCount = 1024;

        // Resources
        std::unique_ptr<GPUParticleResourceCreator> resourceCreator;

        // Mapped data pointers
        EmitterSphere* emitSphereData = nullptr;
        PerView* perViewData          = nullptr;
    };

public:
    GPUParticleManager()  = default;
    ~GPUParticleManager() = default;

    static GPUParticleManager* GetInstance();

    // Init,Update,Draw
    void Init(SrvManager* srvManager);
    void Update();
    void Draw(const ViewProjection& viewProjection);

    // Group creation
    void CreateParticleGroup(
        const std::string& name,
        const std::string& modelFilePath,
        const int32_t& maxCount = 1024);

    void CreatePrimitiveParticle(
        const std::string& name,
       const PrimitiveType& type,
        const int32_t& maxCount);

    // Emit
    void Emit(const std::string& name);

private:
    void InitializeGroupResources(GPUParticleGroup& group);
    void DispatchEmit(GPUParticleGroup& group);
    void DispatchUpdate(GPUParticleGroup& group);
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

    // Setter
    void SetTextureHandle(const std::string& name, const uint32_t& handle);
    void SetModel(const std::string& name, const std::string& modelName);
    void SetEmitterSphere(const std::string& name, const EmitterSphere& emitter);
    void SetViewProjection(const ViewProjection* view);
};