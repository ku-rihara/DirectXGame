#pragma once

#include <vector>
#include <list>
#include <memory>
#include "Vector3.h"
#include "3d/WorldTransform.h"


class Rail {
private:
    const size_t IndexCount_ = 20;                      // レール補間点の数
    std::vector<Vector3> controlPosies_;                // 制御点
    std::vector<Vector3> pointsDrawing_;                // 補間された描画点
    float totalRailLength_ = 0.0f;                      // レールの全体長
    std::list<WorldTransform> railTransforms_;          // レールインスタンス用の変換データ

public:

    Rail() = default;
    ~Rail() = default;

    /// 初期化
    void Init(size_t numObjects);
    /// 更新処理
    void Update(const std::vector<Vector3>& controlPos);
    /// 描画
    void Draw(const ViewProjection& viewProjection);
    /// レール上の位置を取得
    Vector3 GetPositionOnRail(float progress) const;
    // 補間点の取得
    const std::vector<Vector3>& GetPointsDrawing() const { return pointsDrawing_; }
    // 総レール長の取得
    float GetTotalLength() const { return totalRailLength_; }
};
