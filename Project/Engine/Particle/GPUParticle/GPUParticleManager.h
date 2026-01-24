#pragma once
#include "3d/ViewProjection.h"
#include "Base/Descriptors/SrvManager.h"
#include "Data/ParticleCSData.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/Material/GPUParticleMaterial.h"
#include "3D/Primitive/IPrimitive.h"
#include "ResourceData/GPUParticleResourceData.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace KetaEngine {

class Model;
class IPrimitive;
class ViewProjection;

/// <summary>
/// GPUパーティクルシステム管理クラス
/// </summary>
class GPUParticleManager {
public:
    /// <summary>
    /// パーティクルグループ構造体
    /// </summary>
    struct GPUParticleGroup {
        Model* model                           = nullptr;
        std::unique_ptr<IPrimitive> primitive_ = nullptr;
        GPUParticleMaterial material;
        uint32_t textureHandle   = 0;
        int32_t maxParticleCount = 1024;

        std::unique_ptr<GPUParticleResourceData> resourceData;

        ParticleEmit* emitSphereData = nullptr;
        PerView* perViewData          = nullptr;
    };

public:
    GPUParticleManager()  = default;
    ~GPUParticleManager() = default;

    static GPUParticleManager* GetInstance();

    // 初期化、更新、描画
    void Init(SrvManager* srvManager);
    void Update();
    void Draw(const ViewProjection& viewProjection);

    /// <summary>
    /// モデルベースのパーティクルグループを作成
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="modelFilePath">モデルファイルパス</param>
    /// <param name="maxCount">最大パーティクル数</param>
    void CreateParticleGroup(const std::string& name,const std::string& modelFilePath,int32_t maxCount = 1024);

    /// <summary>
    /// プリミティブベースのパーティクルグループを作成
    /// </summary>
    /// <param name="name">グループ名</param>
    /// <param name="type">プリミティブタイプ</param>
    /// <param name="maxCount">最大パーティクル数</param>
    void CreatePrimitiveParticle(const std::string& name,const PrimitiveType& type, int32_t maxCount);

    /// <summary>
    /// 指定したグループのパーティクルを放出
    /// </summary>
    /// <param name="name">グループ名</param>
    void Emit(const std::string& name);
  
private:
    /// <summary>
    /// グループリソースの初期化
    /// </summary>
    /// <param name="group">パーティクルグループ</param>
    void InitializeGroupResources(GPUParticleGroup& group);

    /// <summary>
    /// マテリアルリソースを作成
    /// </summary>
    /// <param name="name">グループ名</param>
    void CreateMaterialResource(const std::string& name);

    /// <summary>
    /// グループの描画
    /// </summary>
    /// <param name="group">パーティクルグループ</param>
    void DrawGroup(GPUParticleGroup& group);

     // DisPatch処理
    void DispatchEmit(GPUParticleGroup& group);
    void DispatchUpdate(GPUParticleGroup& group);
    void DispatchInitParticle(GPUParticleGroup& group);

private:
    DirectXCommon* dxCommon_              = nullptr;
    SrvManager* srvManager_               = nullptr;
    const ViewProjection* viewProjection_ = nullptr;

    std::unordered_map<std::string, GPUParticleGroup> particleGroups_;

public:
    GPUParticleGroup* GetParticleGroup(const std::string& name);
    void SetTextureHandle(const std::string& name, uint32_t handle);
    void SetModel(const std::string& name, const std::string& modelName);
    void SetEmitterSphere(const std::string& name, const ParticleEmit& emitter);
    void SetViewProjection(const ViewProjection* view);
};

}; // KetaEngine
