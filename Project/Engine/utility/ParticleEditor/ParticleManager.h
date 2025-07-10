#pragma once
#include"Dx/DirectXCommon.h"
#include "base/Material.h"
#include "base/SrvManager.h"

#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

//
#include "ParticleEmitter.h"
#include "struct/ParticleForGPU.h"


// math
#include "Box.h"
#include "MinMax.h"
// std
#include <list>
#include <unordered_map>
#include <memory>

struct ParticleEmitter::GroupParamaters;
struct ParticleEmitter::Parameters;
struct ParticleEmitter::EaseParm;
class ParticleManager {

private:
    ///============================================================
    /// struct
    ///============================================================

    struct ScaleInFo {
        Vector3 tempScaleV3;
        Vector3 easeEndScale;
        ParticleEmitter::EaseParm easeParam;
    };
    struct UVInfo {
        Vector3 pos;
        Vector3 scale;
        Vector3 rotate;
        float frameDistance_;
        float frameScroolSpeed;
        float uvStopPos_;
        float currentScroolTime;
        bool isScroolEachPixel;
        bool isScrool;
        bool isRoop;
        bool isFlipX;
        bool isFlipY;
    };

    struct Particle {
        float lifeTime_;
        float currentTime_;
        float gravity_;
        float speed_;
        float easeTime;
        bool isFloatVelocity;
        Vector3 offSet;
        Vector3 direction_;
        Vector3 speedV3;
        Vector3 velocity_;
        Vector3 rotateSpeed_;
        Vector4 color_;
        const Vector3* followPos = nullptr;
        WorldTransform worldTransform_;
        ScaleInFo scaleInfo;
        UVInfo uvInfo_;
    };

    struct AccelerationField { /// 　加速フィールド
        Vector3 acceleration;
        AABB area;
        bool isAdaption;
    };

    struct ParticleGroup { /// パーティクルグループ
        Model* model = nullptr;
        std::unique_ptr<IPrimitive> primitive_=nullptr;
        Material material;
        uint32_t instanceNum;
        uint32_t srvIndex;
        uint32_t textureHandle;
        ParticleFprGPU* instancingData;
        std::list<Particle> particles;
        ParticleEmitter::GroupParamaters param;
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
    };

public:
    ///============================================================
    /// public method
    ///============================================================
    ParticleManager() = default;
    ~ParticleManager() = default;

    // 初期化
    void Init(SrvManager* srvManager);
    void ResetInstancingData(const std::string& name);

    //更新、描画
    void Update();
    void UpdateUV(UVInfo& uvInfo, float deltaTime);
    void Draw(const ViewProjection& viewProjection);
   
    // モデル、リソース作成(グループ作成)
    void CreateParticleGroup(const std::string name, const std::string modelFilePath, const uint32_t& maxnum);
    void CreatePrimitiveParticle(const std::string& name, PrimitiveType type, const uint32_t& maxnum);

    void SetModel(const std::string& name, const std::string& modelName);
    void CreateMaterialResource(const std::string& name);
    void CreateInstancingResource(const std::string& name, const uint32_t& instanceNum);

    /// リセット、パーティクル作成、エミット
    void ResetAllParticles();
    Particle MakeParticle(const ParticleEmitter::Parameters& paramaters);
    void Emit(std::string name, const ParticleEmitter::Parameters& paramaters,
        const ParticleEmitter::GroupParamaters& groupParamaters, const int32_t& count);

    ///============================================================
    /// param Adapt
    ///============================================================
    void AlphaAdapt(ParticleFprGPU& data, const Particle& parm, const ParticleGroup& group);
    Vector3 ScaleAdapt(const float& time, const ScaleInFo& parm);
    Vector3 EaseAdapt(const ParticleEmitter::EaseType& easetype, const Vector3& start,
        const Vector3& end, const float& time, const float& maxTime);

private:
    ///============================================================
    /// private variant
    ///============================================================

    // other class
    SrvManager* pSrvManager_;
    AccelerationField accelerationField_;
    const ViewProjection* viewProjection_;

    /// Particle File
    std::vector<std::string> particleFiles_; // パーティクルのファイル達
    const std::string dyrectry_ = "Resources/GlobalParameter/Particle";

public:
    std::unordered_map<std::string, ParticleGroup> particleGroups_;

public:
    static ParticleManager* GetInstance();

    ///============================================================
    /// getter method
    ///============================================================
    const std::vector<std::string>& GetParticleFiles() const { return particleFiles_; }
    const std::string& getDirectory() const { return dyrectry_; }
    ///============================================================
    /// setter method
    ///============================================================
    void SetViewProjection(const ViewProjection* view);
    void SetTextureHandle(const std::string name, const uint32_t& handle);
    void SetAllParticleFile();
};