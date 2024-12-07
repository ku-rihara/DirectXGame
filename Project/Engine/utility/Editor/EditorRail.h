#pragma once

#include <vector>
#include <memory>
#include "Vector3.h"
#include "3d/WorldTransform.h"
#include "3d/Object3d.h"

class EditorRail {
private:
    const size_t IndexCount_ = 2500;                    // レール補間点の数
    std::vector<Vector3> controlPos_;                  // 制御点
    std::vector<Vector3> pointsDrawing_;               // 補間された描画点
    float totalRailLength_ = 0.0f;                     // レールの全体長
    std::vector<std::unique_ptr<Object3d>> object3d_;  // レール上の各オブジェクト
    std::vector<WorldTransform> transform_;            // 各オブジェクトの変換情報

public:
    ///=========================================================================================
    ///  public 
    ///=========================================================================================
    void Init(size_t ObjNum);
    void Update(const std::vector<Vector3>& controlPos);
    void Draw(const ViewProjection& viewProjection);

    Vector3 RotateVectorAroundAxis(const Vector3& vec, const Vector3& axis, float angle);
    Vector3 GetPositionOnRail(float progress) const;

    const std::vector<Vector3>& GetPointsDrawing() const { return pointsDrawing_; }
    float GetTotalLength() const { return totalRailLength_; }

    // 各オブジェクトの変換データを取得
    const std::vector<WorldTransform>& GetTransforms() const { return transform_; }
};
