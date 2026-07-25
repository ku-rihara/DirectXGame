#pragma once

#include "PostEffect/Distortion.h"
#include "PostEffect/DistortionRenderTarget.h"
#include <cstdint>
#include <d3d12.h>

namespace KetaEngine {

class DirectXCommon;

/// <summary>
/// 時空歪みシステムの管理クラス
/// </summary>
class DistortionManager {
private:
    DistortionManager()  = default;
    ~DistortionManager() = default;

    // コピー禁止
    DistortionManager(const DistortionManager&)            = delete;
    DistortionManager& operator=(const DistortionManager&) = delete;

public:
    static DistortionManager* GetInstance();

    /// <summary>
    /// 歪みRT・ポストエフェクトの初期化
    /// </summary>
    void Init(DirectXCommon* dxCommon, uint32_t width, uint32_t height);

    /// <summary>
    /// 歪みパス開始
    /// </summary>
    void BeginPass();

    /// <summary>
    /// 歪みパス終了
    /// </summary>
    void EndPass();

    /// <summary>
    /// 歪みポストエフェクトを画面に合成する
    /// </summary>
    void ApplyEffect(ID3D12GraphicsCommandList* commandList);

private:

    // 歪みRTと歪みポストエフェクト
    DistortionRenderTarget distortionRT_;
    Distortion             distortionEffect_;

    // 初期化済みかどうか
    bool initialized_     = false;
    // 歪みパスが開かれているかどうか
    bool passOpen_        = false;
    // 歪みを使っているオブジェクトが1つでもあるかどうか
    bool hasAnyDistortion_ = false;
};

} // namespace KetaEngine
