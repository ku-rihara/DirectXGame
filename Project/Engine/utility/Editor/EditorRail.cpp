#include "EditorRail.h"
#include "MathFunction.h"
#include "base/SrvManager.h"

void EditorRail::Init(SrvManager* srvManager, size_t numObjects) {
    // レールオブジェクト（インスタンス用）の初期化
    railObject_.reset(Object3dSRV::CreateModel("EditorRail", ".obj", uint32_t(numObjects), srvManager));
    railObject_->UpdateTransform();
}

void EditorRail::Update(const std::vector<Vector3>& controlPos) {
    controlPos_ = controlPos;
    pointsDrawing_.clear();
    totalRailLength_ = 0.0f;

    // レールの描画点を生成（Catmull-Rom補間を使用）
    for (size_t i = 0; i <= IndexCount_; ++i) {
        float t = static_cast<float>(i) / IndexCount_;
        Vector3 pos = CatmullRomPosition(controlPos_, t);
        pointsDrawing_.push_back(pos);

        if (i > 0) {
            totalRailLength_ += Vector3::Length(pointsDrawing_[i] - pointsDrawing_[i - 1]);
        }
    }

    // 各オブジェクトの位置と回転を計算
    float segmentLength = totalRailLength_ / float(railObject_->GetKnumInstance() - 1);
    float currentLength = 0.0f;
    size_t currentIndex = 0;

    //railObject_->Clear(); // 既存のパーティクルデータをクリア

    for (size_t i = 0; i < railObject_->GetKnumInstance(); i++) {
        while (currentIndex < pointsDrawing_.size() - 1 &&
            currentLength + Vector3::Length(pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]) < segmentLength * i) {
            currentLength += Vector3::Length(pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]);
            currentIndex++;
        }

        if (currentIndex >= pointsDrawing_.size() - 1) {
            break; // 範囲外アクセスを防ぐ
        }

        float t = (segmentLength * i - currentLength) / Vector3::Length(pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]);
        Vector3 interpolatedPos = Lerp(pointsDrawing_[currentIndex], pointsDrawing_[currentIndex + 1], t);

        Vector3 direction = pointsDrawing_[currentIndex + 1] - interpolatedPos;
        direction = Vector3::Normalize(direction);
        float rotateY = std::atan2(direction.x, direction.z);
        float rotateX = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));

        // Transformデータを更新
        WorldTransform transform;
        transform.rotation_ = { rotateX, rotateY, 0.0f };
        transform.translation_ = interpolatedPos;
        transform.UpdateMatrix();

        railObject_->particles_.push_back(std::move(transform));  // ムーブ

        railObject_->UpdateTransform();  // インスタンスデータを更新
    }
}

Vector3 EditorRail::GetPositionOnRail(float progress) const {
    float distance = progress * totalRailLength_;
    float accumulatedDistance = 0.0f;

    for (size_t i = 0; i < pointsDrawing_.size() - 1; ++i) {
        float segmentLength = Vector3::Length(pointsDrawing_[i + 1] - pointsDrawing_[i]);
        if (accumulatedDistance + segmentLength >= distance) {
            float segmentProgress = (distance - accumulatedDistance) / segmentLength;
            return Lerp(pointsDrawing_[i], pointsDrawing_[i + 1], segmentProgress);
        }
        accumulatedDistance += segmentLength;
    }
    return pointsDrawing_.back(); // 最終位置（進行度が1.0fの時）

   
}

void EditorRail::Draw(const ViewProjection& viewProjection) {
    if (railObject_) {
        railObject_->Draw(viewProjection);   // 描画
    }
}
