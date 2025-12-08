#pragma once

#include "3d/WorldTransform.h"
#include "Vector3.h"
#include <list>
#include <memory>
#include <vector>

/// <summary>
/// レール
/// </summary>
class Rail {
private:
    const size_t IndexCount_ = 20;
    std::vector<Vector3> controlPosies_;
    std::vector<Vector3> pointsDrawing_;
    float totalRailLength_ = 0.0f;
    std::list<WorldTransform> railTransforms_;

public:
    Rail()  = default;
    ~Rail() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="numObjects">オブジェクト数</param>
    void Init(const size_t& numObjects);

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="controlPos">制御点座標</param>
    void Update(const std::vector<Vector3>& controlPos);

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Draw(const ViewProjection& viewProjection);

    /// <summary>
    /// レール上の位置を取得
    /// </summary>
    /// <param name="progress">進行度(0.0~1.0)</param>
    /// <returns>レール上の座標</returns>
    Vector3 GetPositionOnRail(float progress) const;

    const std::vector<Vector3>& GetPointsDrawing() const { return pointsDrawing_; }
    float GetTotalLength() const { return totalRailLength_; }
};