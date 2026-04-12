#pragma once

#include "3d/ViewProjection.h"
#include "PostEffect/DistortionRenderTarget.h"
#include "PostEffect/Distortion.h"
#include <d3d12.h>
#include <unordered_set>

namespace KetaEngine {

class DirectXCommon;
class RibbonTrail;

/// <summary>
/// リボントレイル管理クラス
/// </summary>
class RibbonTrailManager {
private:
    RibbonTrailManager()  = default;
    ~RibbonTrailManager();

    RibbonTrailManager(const RibbonTrailManager&)            = delete;
    RibbonTrailManager& operator=(const RibbonTrailManager&) = delete;

public:
    static RibbonTrailManager* GetInstance();

    void Register(RibbonTrail* trail);
    void Unregister(RibbonTrail* trail);

    /// <summary>
    /// 全トレイルを更新してから描画
    /// </summary>
    void DrawAll(const ViewProjection& viewProj);

    /// <summary>
    /// 時空歪みシステムの初期化（DirectX初期化後に一度呼ぶ）
    /// </summary>
    void InitDistortion(DirectXCommon* dxCommon, uint32_t width, uint32_t height);

    /// <summary>
    /// 時空歪みパス描画（DrawAll後・PreDraw前に呼ぶ）
    /// hasExternalDistortion: パーティクルなど外部の歪み描画がある場合true
    /// </summary>
    void DrawDistortionPass(const ViewProjection& viewProj, bool hasExternalDistortion = false);

    /// <summary>
    /// 時空歪みパス終了（DrawDistortionPass後に外部描画がある場合に呼ぶ）
    /// </summary>
    void CloseDistortionPass();

    /// <summary>
    /// 時空歪みポストエフェクト適用（DrawPostEffect内で呼ぶ）
    /// </summary>
    void ApplyDistortionEffect(ID3D12GraphicsCommandList* commandList);

    void Clear();

private:
    static bool isDestroyed_;
    std::unordered_set<RibbonTrail*> trails_;

    // 時空歪み
    DistortionRenderTarget distortionRT_;
    Distortion             distortionEffect_;
    bool distortionInitialized_ = false;
    bool hasAnyDistortion_      = false;
    bool distortionPassOpen_    = false;
};

} // namespace KetaEngine
