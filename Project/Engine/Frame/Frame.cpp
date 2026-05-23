// Frame.cpp
#include "Frame.h"

using namespace KetaEngine;
#include <thread>
#include <immintrin.h>

std::chrono::steady_clock::time_point Frame::reference_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Frame::lastTime_  = std::chrono::steady_clock::now();
float Frame::deltaTime_                                 = 0.0f;
float Frame::deltaTimeRate_                             = 0.0f;
float Frame::timeScale_                                 = 1.0f;
const float Frame::kFPS                                 = 60.0f;

void Frame::Init() {
    reference_ = std::chrono::steady_clock::now();
    lastTime_  = reference_;
    deltaTime_ = 0.0f;
    timeScale_ = 1.0f;
}

void Frame::Update() {
    FixFPS();

    // 経過時間（秒）を計算
    auto currentTime                       = std::chrono::steady_clock::now();
    std::chrono::duration<float> frameTime = currentTime - lastTime_;
    
    // シーン遷移やフリーズによる極端な値を制限（1/10秒以上はロード中とみなす）
    float frameTimeCount = frameTime.count();
    if (frameTimeCount > 0.1f) {
        deltaTime_ = 1.0f / kFPS;
    } else {
        deltaTime_ = frameTimeCount;
    }
    
    deltaTimeRate_ = deltaTime_ * timeScale_;
    lastTime_      = currentTime;
}

float Frame::DeltaTime() {
    return deltaTime_;
}

float Frame::DeltaTimeRate() {
    return deltaTimeRate_;
}

void Frame::FixFPS() {
    // VSync(60Hz)との競合を避けるため、CPU側は少し早めの65FPSをターゲットにする
    const std::chrono::microseconds kTargetTime(uint64_t(1000000.0f / 65.0f));

    // 現在時刻を取得
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // 前回基準からの経過時間を取得
    std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    // ターゲット時間に達していない場合
    if (elapsed < kTargetTime) {
        // 余裕がある場合のみスリープ
        if (kTargetTime - elapsed > std::chrono::microseconds(1000)) {
            std::this_thread::sleep_for(std::chrono::microseconds(500));
        }
        // 残りはビジーループで微調整
        while (std::chrono::steady_clock::now() - reference_ < kTargetTime) {
            _mm_pause();
        }
    }

    // 次のフレームの基準を今に更新
    reference_ = std::chrono::steady_clock::now();
}

/// <summary>
/// 秒からフレームに変換
/// </summary>
int32_t Frame::TimeToFrame(float seconds) {
    return static_cast<int32_t>(seconds * kFPS);
}

/// <summary>
/// フレームから秒に変換
/// </summary>
float Frame::FrameToTime(int32_t frame) {
    return static_cast<float>(frame) / kFPS;
}

void Frame::SetTimeScale(float scale) {

    timeScale_ = scale;
}

float Frame::GetTimeScale() {
    return timeScale_;
}

void Frame::ResetDeltaTime() {
    deltaTime_ = 0.0f;
}
