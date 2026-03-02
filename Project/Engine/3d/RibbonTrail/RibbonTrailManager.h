#pragma once

#include "3d/ViewProjection.h"
#include <unordered_set>

namespace KetaEngine {

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

    void Clear();

private:
    static bool isDestroyed_;
    std::unordered_set<RibbonTrail*> trails_;
};

} // namespace KetaEngine
