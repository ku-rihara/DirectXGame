#include "DebugCamera.h"
#include <numbers>

// カメラ注視点までの距離の初期化
const float DebugCamera::distance_ = 23.0f;

DebugCamera::DebugCamera(int window_width, int window_height) {
    // Inputクラスのインスタンスを取得（仮）
    input_ = Input::GetInstance();
    // ビュープロジェクションの初期化
    viewProjection_.Init();
    matRot_ = MakeIdentity4x4();
    // ビュープロジェクションのパラメータを設定
    SetAspectRatio(static_cast<float>(window_width) / window_height);
    // 初期行列の更新
    UpdateMatrix();
}

void DebugCamera::Init() {
    // ビュープロジェクションの初期化
    viewProjection_.Init();
    // カメラの初期位置を設定
    viewProjection_.translation_ = {0, -5, -distance_};
    viewProjection_.rotation_    = {0, 0, 0};
    matRot_                      = MakeIdentity4x4();
    yaw_                         = 0.0f;
    pitch_                       = 0.0f;
    // 初期行列の更新
    UpdateMatrix();
}

void DebugCamera::Update() {
    // マウスの移動量を取得
    MouseMove mouseMove = input_->GetMouseMove();

    if (input_->IsPressMouse(2)) {
        // カメラのローカル座標系に合わせて平行移動する
        Vector3 right = TransformNormal({1, 0, 0}, matRot_); // カメラの右方向
        Vector3 up    = TransformNormal({0, 1, 0}, matRot_); // カメラの上方向
        viewProjection_.translation_ -= right * (mouseMove.lX * 0.005f); // X軸方向の移動
        viewProjection_.translation_ += up * (mouseMove.lY * 0.005f); // Y軸方向の移動
    }

    if (input_->GetWheel()) {
        // カメラの前方向に基づいた移動
        Vector3 forward = TransformNormal({0, 0, 1}, matRot_);
        viewProjection_.translation_ += forward * (mouseMove.lZ * 0.005f);
    }

    // マウスの右ボタンで視点移動
    if (input_->IsPressMouse(1)) {
        yaw_ += mouseMove.lX * 0.005f;
        pitch_ += mouseMove.lY * 0.005f;

        // ピッチ角の制限（-89度〜+89度）- ジンバルロックを防ぐため90度より少し小さく制限
        const float maxPitch = std::numbers::pi_v<float> / 2.0f - 0.01f; // 約89度
        pitch_               = std::fmax(std::fmin(pitch_, maxPitch), -maxPitch);

        // Yaw角を0-2πの範囲に正規化（オプション）
        if (yaw_ > std::numbers::pi_v<float> * 2.0f) {
            yaw_ -= std::numbers::pi_v<float> * 2.0f;
        }
        if (yaw_ < 0) {
            yaw_ += std::numbers::pi_v<float> * 2.0f;
        }

        // 回転行列の更新 - 順序を変更してより安定した回転を実現
        Matrix4x4 rotY = MakeRotateYMatrix(yaw_);
        Matrix4x4 rotX = MakeRotateXMatrix(pitch_);
        matRot_        = rotY * rotX; // Yaw → Pitch の順序で適用
    }

    UpdateMatrix();
}

void DebugCamera::UpdateMatrix() {
    // カメラ位置に基づいて回転を適用
    Matrix4x4 translateMatrix = MakeTranslateMatrix(viewProjection_.translation_);
    // カメラの注視点を原点とする回転変換
    viewProjection_.cameraMatrix_ = matRot_ * translateMatrix;
    viewProjection_.matView_      = Inverse(viewProjection_.cameraMatrix_);
}