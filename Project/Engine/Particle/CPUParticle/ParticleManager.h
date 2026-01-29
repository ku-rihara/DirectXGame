#pragma once
#include "Base/Descriptors/SrvManager.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Material/ParticleMaterial.h"

#include "3d/Object3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include "Editor/Parameter/ParticleCommonParameters.h"
#include "3D/Primitive/IPrimitive.h"
#include "struct/ParticleForGPU.h"

// Easing
#include "Easing/Easing.h"

// math
#include "Box.h"
#include "MinMax.h"
// std

#include <list>
#include <memory>
#include <unordered_map>

namespace KetaEngine {

/// <summary>
/// パーティクルマネージャー
/// </summary>
class ParticleManager {
public:
    // 共通パラメータの型エイリアス
    using GroupParameters = ParticleCommon::GroupParameters;
    using Parameters      = ParticleCommon::Parameters;
    using EaseParm        = ParticleCommon::ScaleEaseParam;

public:
    struct ScaleInFo {
        Vector3 tempScaleV3;
        Vector3 easeEndScale;
        EaseParm easeParam;
    };

 
    struct TranslateInfo {
        Vector3 startPosition;
        Vector3 endPosition;
    };

    struct RotateInfo {
        Vector3 startRotation;
        Vector3 endRotation;
    };

    struct UVInfo {
        Vector3 pos;
        Vector3 scale;
        Vector3 rotate;
        float frameDistance_;
        float frameScrollSpeed;
        float uvStopPos_;
        float currentScrollTime;
        bool isScrollEachPixel;
        bool isScroll;
        bool isLoop;
        bool isFlipX;
        bool isFlipY;
    };

    struct Particle {
        float lifeTime_;
        float currentTime_;
        float gravity_;
        float speed_;
        bool isFloatVelocity;
        Vector3 offSet;
        Vector3 direction_;
        Vector3 speedV3;
        Vector3 velocity_;
        Vector3 rotateSpeed_;
        Vector4 color_;
        const Vector3* followPos = nullptr;
        std::unique_ptr<WorldTransform> worldTransform_;

        // スケール情報
        ScaleInFo scaleInfo;
        bool isAdaptEasing = false;
        std::unique_ptr<Easing<Vector3>> scaleEasing;

        // Translate情報 
        TranslateInfo translateInfo;
        bool isAdaptTranslateEasing = false;
        std::unique_ptr<Easing<Vector3>> translateEasing;

        // Rotate情報
        RotateInfo rotateInfo;
        bool isAdaptRotateEasing = false;
        std::unique_ptr<Easing<Vector3>> rotateEasing;

        // UV情報
        UVInfo uvInfo_;
    };

    struct AccelerationField {
        Vector3 acceleration;
        AABB area;
        bool isAdaption;
    };

    struct ParticleGroup {
        Model* model                           = nullptr;
        std::unique_ptr<IPrimitive> primitive_ = nullptr;
        ParticleMaterial material;
        uint32_t instanceNum;
        uint32_t srvIndex;
        uint32_t currentNum;
        uint32_t textureHandle;
        ParticleFprGPU* instancingData;
        std::list<Particle> particles;
        GroupParameters param;
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
    };

public:
    ParticleManager()  = default;
    ~ParticleManager() = default;

    // 初期化、更新、描画
    void Init(SrvManager* srvManager);
    void Update();
    void Draw(const ViewProjection& viewProjection);

    void ResetInstancingData(const std::string& name);
    void UpdateUV(UVInfo& uvInfo, float deltaTime);
    void CreateParticleGroup(const std::string name, const std::string modelFilePath, uint32_t maxnum);
    void CreatePrimitiveParticle(const std::string& name, PrimitiveType type, uint32_t maxnum);
    void SetModel(const std::string& name, const std::string& modelName);
    void CreateMaterialResource(const std::string& name);
    void CreateInstancingResource(const std::string& name, uint32_t instanceNum);
    void ResetAllParticles();

    Particle MakeParticle(const Parameters& parameters);
    void Emit(std::string name, const Parameters& parameters,
        const GroupParameters& groupParameters, int32_t count);

    void AlphaAdapt(ParticleFprGPU& data, const Particle& parm, const ParticleGroup& group);

private:
    SrvManager* pSrvManager_;
    AccelerationField accelerationField_;
    const ViewProjection* viewProjection_;

    std::vector<std::string> particleFiles_;
    const std::string ParticleFolderName_ = "Particle";

public:
    std::unordered_map<std::string, ParticleGroup> particleGroups_;

    static ParticleManager* GetInstance();

    const std::vector<std::string>& GetParticleFiles() const { return particleFiles_; }
    const std::string& getDirectory() const { return ParticleFolderName_; }

    void SetViewProjection(const ViewProjection* view);
    void SetTextureHandle(const std::string name, uint32_t handle);
    void SetAllParticleFile();
};

}; // KetaEngine