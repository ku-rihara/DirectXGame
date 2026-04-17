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
// Dissolve
#include "Editor/DissolveEditor/DissolvePlayer.h"

// math
#include "Box.h"
#include "MinMax.h"
// std

#include <list>
#include <memory>
#include <unordered_map>

namespace KetaEngine {

// forward declarations
class ParticleFactory;
class ParticleUpdater;
class ParticleRenderer;
class ParticleGroupRegistry;

class ParticleManager {
public:
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

    struct ColorInfo {
        Vector3 startColor;
        Vector3 endColor;
    };

    struct UVInfo {
        Vector3 pos             = {};
        Vector3 scale           = {1.0f, 1.0f, 1.0f};
        Vector3 rotate          = {};
        float frameDistance_    = 0.0f;
        float frameScrollSpeed  = 0.0f;
        float uvStopPos_        = 0.0f;
        float currentScrollTime = 0.0f;
        bool isLoop             = false;
        bool isFlipX            = false;
        bool isFlipY            = false;
        int32_t numOfFrame      = 0; ///< UV スクロール有効判定用
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

        ScaleInFo scaleInfo;
        bool isAdaptEasing = false;
        std::unique_ptr<Easing<Vector3>> scaleEasing;

        TranslateInfo translateInfo;
        bool isAdaptTranslateEasing = false;
        std::unique_ptr<Easing<Vector3>> translateEasing;

        RotateInfo rotateInfo;
        bool isAdaptRotateEasing = false;
        std::unique_ptr<Easing<Vector3>> rotateEasing;

        ColorInfo colorInfo;
        bool isAdaptColorEasing = false;
        std::unique_ptr<Easing<Vector3>> colorEasing;

        ParticleCommon::AlphaMode alphaMode = ParticleCommon::AlphaMode::LifeTime;
        bool isAdaptAlphaEasing = false;
        std::unique_ptr<Easing<float>> alphaEasing;

        UVInfo uvInfo_;

        float dissolveCurrentTime  = 0.0f;
        float dissolveOffsetTime   = 0.0f;
        bool isAdaptDissolveEasing = false;
        std::unique_ptr<float> dissolveThresholdData_;
        std::unique_ptr<Easing<float>> dissolveEasing;
    };

    struct AccelerationField {
        Vector3 acceleration;
        AABB area;
        bool isAdaption;
    };

    struct DissolveGroupParams {
        float startThreshold = 1.0f;
        float endThreshold   = 0.0f;
        float maxTime        = 1.0f;
        float offsetTime     = 0.0f;
        int32_t easeType     = 0;
        bool isActive        = false;
    };

    struct ParticleGroup {
        Model* model                           = nullptr;
        std::unique_ptr<IPrimitive> primitive_ = nullptr;
        ParticleMaterial material;
        uint32_t instanceNum;
        uint32_t srvIndex;
        uint32_t currentNum;
        uint32_t textureHandle;
        uint32_t dissolveTextureHandle    = 0;
        uint32_t distortionTextureHandle  = 0; // 歪みノイズテクスチャ
        ParticleFprGPU* instancingData;
        std::list<Particle> particles;
        GroupParameters param;
        DissolveGroupParams dissolveParams;
        std::unique_ptr<DissolvePlayer> dissolvePlayer;
        std::string lastDissolveTexturePath;
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
    };

public:
    ParticleManager()  = default;
    ~ParticleManager() = default;

    void Init(SrvManager* srvManager);
    void Update();
    void Draw(const ViewProjection& viewProjection);
    void DrawScreenPos();
    void DrawDistortion(const ViewProjection& viewProjection); // 歪みパス描画

    void ResetInstancingData(const std::string& name);
    void UpdateUV(UVInfo& uvInfo, float deltaTime);
    void CreateParticleGroup(const std::string name, const std::string modelFilePath, uint32_t maxnum);
    void CreatePrimitiveParticle(const std::string& name, PrimitiveType type, uint32_t maxnum);
    void SetModel(const std::string& name, const std::string& modelName);
    void CreateMaterialResource(const std::string& name);
    void CreateInstancingResource(const std::string& name, uint32_t instanceNum);
    void ResetAllParticles();

    Particle MakeParticle(const Parameters& parameters, const DissolveGroupParams* dissolveParams = nullptr);
    void Emit(std::string name, const Parameters& parameters,
        const GroupParameters& groupParameters, int32_t count);

    void AlphaAdapt(ParticleFprGPU& data, const Particle& parm);

private:
    SrvManager* pSrvManager_;
    AccelerationField accelerationField_;
    const ViewProjection* viewProjection_;

    std::vector<std::string> particleFiles_;
    const std::string ParticleFolderName_ = "Particle";

    std::unique_ptr<ParticleFactory>       factory_;
    std::unique_ptr<ParticleUpdater>       updater_;
    std::unique_ptr<ParticleRenderer>      renderer_;
    std::unique_ptr<ParticleGroupRegistry> registry_;

public:
    std::unordered_map<std::string, ParticleGroup> particleGroups_;

    static ParticleManager* GetInstance();

    const std::vector<std::string>& GetParticleFiles() const { return particleFiles_; }
    const std::string& getDirectory() const { return ParticleFolderName_; }

    bool HasDistortionParticles() const; // 歪みパーティクルが存在するか確認

    void SetViewProjection(const ViewProjection* view);
    void SetTextureHandle(const std::string name, uint32_t handle);
    void SetDissolveTextureHandle(const std::string& name, uint32_t handle);
    void SetDistortionTextureHandle(const std::string& name, uint32_t handle); // 歪みテクスチャ設定
    void PlayDissolve(const std::string& name, const std::string& dissolveName);
    void StopDissolve(const std::string& name);
    void SetAllParticleFile();
};

}; // KetaEngine
