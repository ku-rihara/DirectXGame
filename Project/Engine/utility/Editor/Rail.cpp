#include "Rail.h"
#include "MathFunction.h"
#include "base/SrvManager.h"

void Rail::Init(size_t numObjects) {
  
    numObjects;
    //// レールオブジェクト（インスタンス用）の初期化
    //railObject_.reset(Object3dSRV::CreateModel("EditorRail", ".obj", uint32_t(numObjects), srvManager));
    //railTransforms_.resize(numObjects); // 必要なインスタンス分だけ確保
}

void Rail::Update(const std::vector<Vector3>& controlPos) {
    controlPosies_ = controlPos;
    pointsDrawing_.clear();
    totalRailLength_ = 0.0f;

    // レールの描画点を生成（Catmull-Rom補間を使用）
    for (size_t i = 0; i <= IndexCount_; ++i) {
        float t = static_cast<float>(i) / IndexCount_;
        Vector3 pos = CatmullRomPosition(controlPosies_, t);
        pointsDrawing_.push_back(pos);

        if (i > 0) {
            totalRailLength_ +=(pointsDrawing_[i] - pointsDrawing_[i - 1]).Length();
        }
    }

    // 各オブジェクトの位置と回転を計算
    float segmentLength = totalRailLength_ / float(railTransforms_.size() - 1);
    float currentLength = 0.0f;
    size_t currentIndex = 0;

    auto it = railTransforms_.begin(); // イテレータを使用
    for (size_t i = 0; it != railTransforms_.end(); ++it, ++i) {
        while (currentIndex < pointsDrawing_.size() - 1 &&
            currentLength + (pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]).Length() < segmentLength * i) {
            currentLength += (pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]).Length();
            currentIndex++;
        }

        if (currentIndex >= pointsDrawing_.size() - 1) {
            break; // 範囲外アクセスを防ぐ
        }

        float t = (segmentLength * i - currentLength) / (pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]).Length();
        Vector3 interpolatedPos = Lerp(pointsDrawing_[currentIndex], pointsDrawing_[currentIndex + 1], t);

        Vector3 direction = pointsDrawing_[currentIndex + 1] - interpolatedPos;
        direction = Vector3::Normalize(direction);
        float rotateY = std::atan2(direction.x, direction.z);
        float rotateX = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));

        // Transformデータを更新
        it->rotation_ = { rotateX, rotateY, 0.0f };
        it->scale_ = { 1, 1, 1 };
        it->translation_ = interpolatedPos;
        it->UpdateMatrix();
    }
}


Vector3 Rail::GetPositionOnRail(float progress) const {
    float distance = progress * totalRailLength_;
    float accumulatedDistance = 0.0f;

    for (size_t i = 0; i < pointsDrawing_.size() - 1; ++i) {
        float segmentLength = (pointsDrawing_[i + 1] - pointsDrawing_[i]).Length();
        if (accumulatedDistance + segmentLength >= distance) {
            float segmentProgress = (distance - accumulatedDistance) / segmentLength;
            return Lerp(pointsDrawing_[i], pointsDrawing_[i + 1], segmentProgress);
        }
        accumulatedDistance += segmentLength;
    }
    return pointsDrawing_.back(); // 最終位置（進行度が1.0fの時）
}

void Rail::Draw(const ViewProjection& viewProjection) {
    viewProjection;
    //if (railObject_) {
    //    // railTransforms_を渡して描画
    //    railObject_->Draw(railTransforms_, viewProjection);
    //}
}
