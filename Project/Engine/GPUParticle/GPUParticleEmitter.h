#pragma once
#include "Data/GPUParticleEmitterData.h"
#include "Vector3.h"
#include <memory>
#include <string>

enum class PrimitiveType;

/// <summary>
/// GPUパーティクルのエミッタークラス
/// </summary>
class GPUParticleEmitter {
public:
    GPUParticleEmitter()  = default;
    ~GPUParticleEmitter() = default;

    /// <summary>
    /// モデルベースのパーティクルを作成
    /// </summary>
    /// <param name="name">パーティクル名</param>
    /// <param name="modelFilePath">モデルファイルパス</param>
    /// <param name="maxCount">最大パーティクル数</param>
    /// <returns>作成されたエミッター</returns>
    static GPUParticleEmitter* CreateParticle(const std::string& name, const std::string& modelFilePath, const int32_t& maxCount = 1024);

    /// <summary>
    /// プリミティブベースのパーティクルを作成
    /// </summary>
    /// <param name="name">パーティクル名</param>
    /// <param name="type">プリミティブタイプ</param>
    /// <param name="maxCount">最大パーティクル数</param>
    /// <returns>作成されたエミッター</returns>
    static GPUParticleEmitter* CreateParticlePrimitive(const std::string& name, const PrimitiveType& type, const int32_t& maxCount = 1024);

    void Init(); //< エミッターの初期化
    void Update(); //< エミッターの更新
    void Emit(); //< パーティクルを放出

    /// <summary>
    /// エミッターデータを設定
    /// </summary>
    /// <param name="emitter">エミッター設定データ</param>
    void SetEmitterData(const EmitterSphere& emitter);

    /// <summary>
    /// テクスチャを設定
    /// </summary>
    /// <param name="textureHandle">テクスチャハンドル</param>
    void SetTexture(uint32_t textureHandle);

private:
    /// <summary>
    /// モデルで初期化
    /// </summary>
    /// <param name="name">パーティクル名</param>
    /// <param name="modelFilePath">モデルファイルパス</param>
    /// <param name="maxCount">最大パーティクル数</param>
    void InitWithModel(const std::string& name, const std::string& modelFilePath, const int32_t& maxCount);

    /// <summary>
    /// プリミティブで初期化
    /// </summary>
    /// <param name="name">パーティクル名</param>
    /// <param name="type">プリミティブタイプ</param>
    /// <param name="maxCount">最大パーティクル数</param>
    void InitWithPrimitive(const std::string& name, const PrimitiveType& type, const int32_t& maxCount);

private:
    std::string name_;
    float currentTime_ = 0.0f;
    float frequency_   = 1.0f;
    uint32_t count_    = 10;
    bool shouldEmit_   = false;

public:
    const std::string& GetName() const { return name_; }
    const bool& IsValid() const { return !name_.empty(); }
    const float& GetFrequencyTime() const { return currentTime_; }
    uint32_t GetEmitFlag() const { return shouldEmit_ ? 1 : 0; }
};