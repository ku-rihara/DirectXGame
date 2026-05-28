// Frame.cpp
#include "Frame.h"

using namespace KetaEngine;
#include <thread>

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
    deltaTime_                             = frameTime.count();
    deltaTimeRate_                         = deltaTime_ * timeScale_;
    lastTime_                              = currentTime;
}

void Frame::FixFPS() {
    // 1/60秒ピッタリの時間
    const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

    // 現在時刻を取得
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    // 1/60秒経っていない場合
    if (elapsed < kMinTime) {
        // 残り時間を計算
        auto sleepTime = kMinTime - elapsed;

        // 残り時間が1ミリ秒以上ある場合は、1ミリ秒引いた時間だけスリープする
        if (sleepTime > std::chrono::microseconds(1000)) {
            std::this_thread::sleep_for(sleepTime - std::chrono::microseconds(1000));
        }

        // 微小な待機を繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            std::this_thread::yield();
        }
    }

    // 現在時刻を基準時間として記録
    reference_ = std::chrono::steady_clock::now();
}

int32_t Frame::TimeToFrame(float seconds) {
    // 秒からフレームに変換
    return static_cast<int32_t>(seconds * kFPS);
}

float Frame::FrameToTime(int32_t frame) {
    // フレームから秒に変換
    return static_cast<float>(frame) / kFPS;
}

float Frame::DeltaTime() {
    return deltaTime_;
}

float Frame::DeltaTimeRate() {
    return deltaTimeRate_;
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
