#include"Mouse.h"
#include <cmath>
#include<assert.h>
#include"base/WinApp.h"
void Mouse::Init(Microsoft::WRL::ComPtr<IDirectInput8>directInput,HWND hWnd) {
    hWnd_ = hWnd;
    //マウスデバイスの生成
  HRESULT  result = directInput->CreateDevice(GUID_SysMouse, &devMouse_, NULL);
    assert(SUCCEEDED(result));
    mousePosition_ = { 0.0f, 0.0f }; // 初期値の確認

    //入力データ形式のセット
    result = devMouse_->SetDataFormat(&c_dfDIMouse2);
    assert(SUCCEEDED(result));

    // 排他制御レベルのセット
    result = devMouse_->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    assert(SUCCEEDED(result));
}

void Mouse::Update() {
    // マウスの状態を更新
    mousePre_ = mouse_;
    devMouse_->Acquire();
    devMouse_->GetDeviceState(sizeof(mouse_), &mouse_);
}

//マウス****************************************************************

bool Mouse::IsPressMouse(int32_t buttonNumber)const {
    return(mouse_.rgbButtons[buttonNumber] & 0x80);
}

bool Mouse::IsTriggerMouse(int32_t buttonNumber)const {
    return (mouse_.rgbButtons[buttonNumber] & 0x80) && !(mousePre_.rgbButtons[buttonNumber] & 0x80);
}

MouseMove Mouse::GetMouseMove() {
    MouseMove move;
    move.lX = mouse_.lX;
    move.lY = mouse_.lY;
    move.lZ = mouse_.lZ;
    return move;
}

Vector3 Mouse::GetMousePos3D(const ViewProjection& viewprojection, float depthFactor, float blockSize) const {
    // 2Dマウス座標を取得
    Vector2 mousePos = mousePosition_;

    // ウィンドウサイズ
    float windowWidth = WinApp::kWindowWidth;
    float windowHeight = WinApp::kWindowHeight;

    // スクリーン座標を正規化デバイス座標 (NDC) に変換 [-1, 1] の範囲にする
    float ndcX = (2.0f * mousePos.x / windowWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePos.y / windowHeight);
    float ndcZ = depthFactor; // Z軸の奥行きを調整するパラメータ

    // NDC座標をVector3に変換（NDCのZ値をdepthFactorで調整）
    Vector3 clipPos = { ndcX, ndcY, ndcZ };

    // 逆射影行列を使ってクリップ空間からビュー空間へ変換
    Matrix4x4 invProj = Inverse(viewprojection.matProjection_);
    Vector3 viewPos = MatrixTransform(clipPos, invProj);

    // ビュー空間からワールド空間へ変換
    Matrix4x4 invView = Inverse(viewprojection.matView_);
    Vector3 worldPos = MatrixTransform(viewPos, invView);

    // ブロックサイズに基づいてスナップ処理を適用
    if (blockSize > 0.0f) {
        worldPos.x = std::round(worldPos.x / blockSize) * blockSize;
        worldPos.y = std::round(worldPos.y / blockSize) * blockSize;
        worldPos.z = std::round(worldPos.z / blockSize) * blockSize;
    }

    // ワールド座標を返す
    return worldPos;
}

int32_t Mouse::GetWheel() const {
    return mouse_.lZ;
}

Vector2 Mouse::GetMousePos() {
    // マウス座標を取得
    POINT mousePos;
    GetCursorPos(&mousePos);
    // スクリーン座標からウィンドウ内座標に変換
    ScreenToClient(hWnd_, &mousePos); // hWndはウィンドウハンドル
    mousePosition_ = Vector2(float(mousePos.x), float(mousePos.y));

    return mousePosition_;
}