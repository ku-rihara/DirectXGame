#include "DebugCamera.h"


// カメラ注視点までの距離の初期化
const float DebugCamera::distance_ = 10.0f;

DebugCamera::DebugCamera(int window_width, int window_height) {
    // Inputクラスのインスタンスを取得（仮）
    input_ = Input::GetInstance();

    // ビュープロジェクションの初期化
    viewProjection_.Init();

    // カメラの初期位置を設定
    viewProjection_.translation_ = { 0, 0, -distance_ };
    viewProjection_.rotation_ = { 0, 0, 0 };
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
    viewProjection_.translation_ = { 0, 0, -distance_ };
    viewProjection_.rotation_ = { 0, 0, 0 };
    matRot_ = MakeIdentity4x4();

    // 初期行列の更新
    UpdateMatrix();
}

void DebugCamera::Update() {
    // マウスの移動量を取得
    MouseMove mouseMove = input_->GetMouseMove();

    if (input_->IsPressMouse(2)) {
        viewProjection_.translation_.x+= mouseMove.lX * 0.005f; // スケール調整
        viewProjection_.translation_.y += mouseMove.lY * 0.005f; // スケール調整
    }
    if (input_->GetWheel()) {
      /* Vector3 direction = TransformNormal({ 0,0,1 }, matRot_);
        direction = Normalize(direction);*/
        viewProjection_.translation_+= (mouseMove.lZ * 0.005f)*Vector3(0,0,1)/**direction*/;
    }

    //マウスの回転
    if (input_->IsPressMouse(1)) {
       //追加回転分の回転行列を生成
        Matrix4x4 matRotDelta = MakeIdentity4x4();
        matRotDelta *= MakeRotateXMatrix(mouseMove.lY * 0.005f);
        matRotDelta*=MakeRotateYMatrix(mouseMove.lX * 0.005f);
        //累積の回転行列を合成
        matRot_ = matRotDelta * matRot_;
    }
    UpdateMatrix();
}


void DebugCamera::UpdateMatrix() {
  /*  viewProjection_.UpdateMatrix();*/
    Matrix4x4 translateMatrix = MakeTranslateMatrix(viewProjection_.translation_);
    Matrix4x4 cameraMatrix =   translateMatrix* matRot_;
    viewProjection_.matView_ = Inverse(cameraMatrix);
}