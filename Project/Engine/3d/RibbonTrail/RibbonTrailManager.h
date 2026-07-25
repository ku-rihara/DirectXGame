#pragma once

#include "3D/ViewProjection.h"
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

    // コピー禁止
    RibbonTrailManager(const RibbonTrailManager&)            = delete;
    RibbonTrailManager& operator=(const RibbonTrailManager&) = delete;

public:
    static RibbonTrailManager* GetInstance();

    // 登録、解除
    void Register(RibbonTrail* trail);
    void Unregister(RibbonTrail* trail);

    /// <summary>
    /// 全トレイルを更新してから描画
    /// </summary>
    void DrawAll(const ViewProjection& viewProj);

    /// <summary>
    /// 歪みが有効なトレイルが1つでもあるか
    /// </summary>
    bool HasDistortionTrails() const;

    /// <summary>
    /// 歪みが有効なトレイルを、現在開かれている歪みRTに描画する。
    /// 歪みRTの開閉・所有はDistortionManagerが担当する。
    /// </summary>
    void DrawDistortionTrails(const ViewProjection& viewProj);

    void Clear();

private:
    static bool isDestroyed_;
    std::unordered_set<RibbonTrail*> trails_;
};

} // namespace KetaEngine
