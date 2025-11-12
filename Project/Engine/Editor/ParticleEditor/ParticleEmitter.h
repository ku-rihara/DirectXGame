#pragma once

#include "3d/ViewProjection.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Editor/RailEditor/RailManager.h"

#include "Line3D/Line3D.h"
#include "Primitive/IPrimitive.h"

#include "ParticleParameter.h"
#include <string>
#include <vector>

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
        const int32_t& maxnum);

    /// <summary>
    /// プリミティブパーティクルの作成
    /// </summary>
    /// <param name="name">名前</param>
    /// <param name="primitiveType">プリミティブタイプ</param>
    /// <param name="maxNum">最大数</param>
    /// <returns>生成されたエミッター</returns>
    static ParticleEmitter* CreateParticlePrimitive(
        const std::string& name, const PrimitiveType& primitiveType, const int32_t& maxnum);
  
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

    void Init();                  //< 初期化
    void Update();                //< 更新
    void Emit();                  //< パーティクルの放出
    void StartRailEmit();         //< レール放出開始
    void UpdateEmitTransform();   //< エミット位置の更新
    void EditorUpdate() override; //< エディタ更新

private:
    void RailMoveUpdate(); //< レール移動更新

private:
    float currentTime_;
    std::unique_ptr<Line3D>debugLine_;
    WorldTransform emitBoxTransform_;
    bool isStartRailMove_;
    std::string editorMessage_;

public:
    // getter
   const bool& GetIsMoveForRail() const { return isMoveForRail_; }
   const float& GetMoveSpeed() const { return moveSpeed_; }
    // setter
    void SetTextureHandle(const uint32_t& hanle);
    void SetParentTransform(const WorldTransform* transform);
    void SetFollowingPos(const Vector3* pos);
    void SetBlendMode(const BlendMode& blendmode);
    void SetBillBordType(const BillboardType& billboardType);
    void SetParentBasePos(WorldTransform* parent);
    void SetIsMoveForRail(const bool& isMoveForRail) { isMoveForRail_ = isMoveForRail; }
    void SetMoveSpeed(const float& moveSpeed) { moveSpeed_ = moveSpeed; }
    void SetTargetPosition(const Vector3& targetPos) { parameters_.targetPos = targetPos; }
};