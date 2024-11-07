#include "Rail.h"
#include "MathFunction.h"
#include <cmath>

void Rail::Init(size_t numObjects) {
    // オブジェクトと変換情報の初期化
    object3d_.resize(numObjects);
    transform_.resize(numObjects);

    for (size_t i = 0; i < numObjects; ++i) {
        object3d_[i].reset(Object3d::CreateModel("Rail",".obj"));
    }
}

void Rail::Update(const std::vector<Vector3>& controlPos) {
    controlPos_ = controlPos;
   /* pointsDrawing_.clear();*/
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

    // 各オブジェクトの位置と回転を設定
    float segmentLength = totalRailLength_ / float(object3d_.size() - 1);
    float currentLength = 0.0f;
    size_t currentIndex = 0;

    for (size_t i = 0; i < object3d_.size(); i++) {
        while (currentIndex < pointsDrawing_.size() - 1 &&
            currentLength + Vector3::Length(pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]) < segmentLength * i) {
            currentLength += Vector3::Length(pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]);
            currentIndex++;
        }

        float t = (segmentLength * i - currentLength) / Vector3::Length(pointsDrawing_[currentIndex + 1] - pointsDrawing_[currentIndex]);
        Vector3 interpolatedPos = Lerp(pointsDrawing_[currentIndex], pointsDrawing_[currentIndex + 1], t);

        Vector3 direction = pointsDrawing_[currentIndex + 1] - interpolatedPos;
        direction = Vector3::Normalize(direction);
        float rotateY = std::atan2(direction.x, direction.z);
        float rotateX = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));
        Vector3 right = Vector3::Cross(Vector3(0, 1, 0), direction);
        right = Vector3::Normalize(right);
        float rotateZ = std::atan2(right.y, right.x);

        transform_[i].rotation_ = { rotateX, rotateY, rotateZ };
        transform_[i].translation_ = interpolatedPos;
        transform_[i].UpdateMatrix();
    }
}

Vector3 Rail::GetPositionOnRail(float progress) const {
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


void Rail::Draw(const ViewProjection& viewProjection) {
    for (size_t i = 0; i < object3d_.size(); ++i) {
        object3d_[i]->Draw(transform_[i], viewProjection);
    }
}