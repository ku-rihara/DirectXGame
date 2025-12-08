#pragma once
#include "base/SrvManager.h"
#include "Dx/DirectXCommon.h"
#include "Material/ParticleMaterial.h"

#include "3d/Object3d.h"
#include "3d/ViewProjection.h"
#include "3d/WorldTransform.h"

#include "ParticleEmitter.h"
#include "struct/ParticleForGPU.h"

// math
#include "Box.h"
#include "MinMax.h"
// std
#include <list>
#include <memory>
#include <unordered_map>

struct ParticleEmitter::GroupParamaters;
struct ParticleEmitter::Parameters;
struct ParticleEmitter::EaseParm;

/// <summary>
/// パーティクルマネージャー
/// </summary>
class ParticleManager {

private:
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
        std::unique_ptr<WorldTransform> worldTransform_; 
        ScaleInFo scaleInfo;
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
        ParticleEmitter::GroupParamaters param;
        Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
    };

public:
    ParticleManager()  = default;
    ~ParticleManager() = default;

    
    // 初期化、更新、描画
    void Init(SrvManager* srvManager);
    void Update();
    void Draw(const ViewProjection& viewProjection);

    /// <summary>
    /// インスタンシングデータのリセット
    /// </summary>
    /// <param name="name">パーティクルグループ名</param>
    void ResetInstancingData(const std::string& name);


    /// <summary>
    /// UV更新
    /// </summary>
    /// <param name="uvInfo">UV情報</param>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateUV(UVInfo& uvInfo, float deltaTime);

    /// <summary>
    /// パーティクルグループの作成
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="modelFilePath">モデルファイルパス</param>
    /// <param name="maxnum">最大パーティクル数</param>
    void CreateParticleGroup(const std::string name, const std::string modelFilePath, uint32_t maxnum);

    /// <summary>
    /// プリミティブパーティクルの作成
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="type">プリミティブタイプ</param>
    /// <param name="maxnum">最大パーティクル数</param>
    void CreatePrimitiveParticle(const std::string& name, PrimitiveType type, uint32_t maxnum);

    /// <summary>
    /// モデルの設定
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="modelName">モデル名</param>
    void SetModel(const std::string& name, const std::string& modelName);

    /// <summary>
    /// マテリアルリソースの作成
    /// </summary>
    /// <param name="name">グループ名</param>
    void CreateMaterialResource(const std::string& name);

    /// <summary>
    /// インスタンシングリソースの作成
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="instanceNum">インスタンス数</param>
    void CreateInstancingResource(const std::string& name, uint32_t instanceNum);

    void ResetAllParticles(); //< 全パーティクルのリセット

    /// <summary>
    /// パーティクルの生成
    /// </summary>
    /// <param name="paramaters">パラメータ</param>
    /// <returns>生成されたパーティクル</returns>
    Particle MakeParticle(const ParticleEmitter::Parameters& paramaters);

    /// <summary>
    /// パーティクルの放出
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="paramaters">パラメータ</param>
    /// <param name="groupParamaters">グループパラメータ</param>
    /// <param name="count">放出数</param>
    void Emit(std::string name, const ParticleEmitter::Parameters& paramaters,
        const ParticleEmitter::GroupParamaters& groupParamaters, int32_t count);

    /// <summary>
    /// アルファ値の適用
    /// </summary>
    /// <param name="data">GPU用データ</param>
    /// <param name="parm">パーティクルパラメータ</param>
    /// <param name="group">パーティクルグループ</param>
    void AlphaAdapt(ParticleFprGPU& data, const Particle& parm, const ParticleGroup& group);

    /// <summary>
    /// スケールの適用
    /// </summary>
    /// <param name="time">経過時間</param>
    /// <param name="parm">スケール情報</param>
    /// <returns>適用後のスケール</returns>
    Vector3 ScaleAdapt(float time, const ScaleInFo& parm);

    /// <summary>
    /// イージングの適用
    /// </summary>
    /// <param name="easetype">イージングタイプ</param>
    /// <param name="start">開始値</param>
    /// <param name="end">終了値</param>
    /// <param name="time">経過時間</param>
    /// <param name="maxTime">最大時間</param>
    /// <returns>イージング後の値</returns>
    Vector3 EaseAdapt(const ParticleEmitter::EaseType& easetype, const Vector3& start,
        const Vector3& end, float time, float maxTime);

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