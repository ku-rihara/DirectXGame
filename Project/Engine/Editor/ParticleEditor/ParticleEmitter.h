#pragma once

#include "3d/ViewProjection.h"
#include "Editor/RailEditor/RailManager.h"
#include "Editor/RailEditor/RailPlayer.h"
#include "Pipeline/Particle/ParticlePipeline.h"

#include "Line3D/Line3D.h"
#include "Primitive/IPrimitive.h"

#include "ParticleParameter.h"
#include <string>
#include <vector>

namespace KetaEngine {

class Object3DAnimation;

/// <summary>
/// パーティクルエミッター
/// </summary>
class ParticleEmitter : public ParticleParameter {

public:
    ParticleEmitter()           = default;
    ~ParticleEmitter() override = default;

    /// <summary>
    /// パーティクルの作成
    /// </summary>
    /// <param name="name">名前</param>
    /// <param name="modelFilePath">モデルファイルパス</param>
    /// <param name="maxNum">最大数</param>
    /// <returns>生成されたエミッター</returns>
    static ParticleEmitter* CreateParticle(
        const std::string& name, const std::string& modelFilePath,
        int32_t maxnum);

    /// <summary>
    /// プリミティブパーティクルの作成
    /// </summary>
    /// <param name="name">名前</param>
    /// <param name="primitiveType">プリミティブタイプ</param>
    /// <param name="maxNum">最大数</param>
    /// <returns>生成されたエミッター</returns>
    static ParticleEmitter* CreateParticlePrimitive(
        const std::string& name, const PrimitiveType& primitiveType, int32_t maxnum);

    /// <summary>
    /// 親ジョイントの設定
    /// </summary>
    /// <param name="modelAnimation">アニメーションモデル</param>
    /// <param name="name">ジョイント名</param>
    void SetParentJoint(const Object3DAnimation* modelAnimation, const std::string& name);

    /// レールのデバッグ描画
    void RailDraw();

    /// <summary>
    /// デバッグ描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void SetEmitLine();

    void Init(); 
    void Update();
    void Emit(); 
    void StartRailEmit();
    void StartRailPlayerEmit(const std::string& railFileName); 
    void StopRailPlayerEmit();
    void UpdateEmitTransform(); 
    void EditorUpdate() override; 

private:
    void RailMoveUpdate(); 
    void RailPlayerUpdate();

private:
    float currentTime_;
    std::unique_ptr<Line3D> debugLine_;
    WorldTransform emitBoxTransform_;
    bool isStartRailMove_;
    std::string editorMessage_;

    // 旧システム用（互換性のため残す）
    std::unique_ptr<RailManager> railManager_;

    // 新システム用
    std::unique_ptr<RailPlayer> railPlayer_;
    bool useRailPlayer_    = false;
    float railPlayerSpeed_ = 1.0f;

public:
    // getter
    bool GetIsMoveForRail() const { return isMoveForRail_; }
    float GetMoveSpeed() const { return moveSpeed_; }
    bool IsUsingRailPlayer() const { return useRailPlayer_; }
    RailPlayer* GetRailPlayer() { return railPlayer_.get(); }

    // setter
    void SetTextureHandle(uint32_t hanle);
    void SetParentTransform(const WorldTransform* transform);
    void SetFollowingPos(const Vector3* pos);
    void SetBlendMode(const BlendMode& blendmode);
    void SetBillBordType(const BillboardType& billboardType);
    void SetParentBasePos(WorldTransform* parent);
    void SetIsMoveForRail(bool isMoveForRail) { isMoveForRail_ = isMoveForRail; }
    void SetMoveSpeed(float moveSpeed) { moveSpeed_ = moveSpeed; }
    void SetTargetPosition(const Vector3& targetPos) { parameters_.targetPos = targetPos; }
    void SetRailPlayerSpeed(float speed) { railPlayerSpeed_ = speed; }
};

}; // KetaEngine
