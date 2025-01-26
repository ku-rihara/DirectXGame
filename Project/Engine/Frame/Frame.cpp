// Frame.cpp
#include "Frame.h"
#include <thread>

std::chrono::steady_clock::time_point Frame::reference_ = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point Frame::lastTime_ = std::chrono::steady_clock::now();
float Frame::deltaTime_ = 0.0f;
float Frame::deltaTimeRate_ = 0.0f;
float Frame::timeScale_ = 1.0f;

void Frame::Init() {
    reference_ = std::chrono::steady_clock::now();
    lastTime_ = reference_;
    deltaTime_ = 0.0f;
    timeScale_ = 1.0f;
}

void Frame::Update() {
    FixFPS(); // FPS制御を実行

    // 経過時間（秒）を計算
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> frameTime = currentTime - lastTime_;
    deltaTime_ = frameTime.count();
    deltaTimeRate_ = deltaTime_ * timeScale_;
    lastTime_ = currentTime;
}

float Frame::DeltaTime() {
    return deltaTime_; // 経過時間を返す
}

float Frame::DeltaTimeRate() {
    return deltaTimeRate_;
}

void Frame::FixFPS() {
    // 1/60秒ピッタリの時間
    const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
    // 1/60秒にわずかに短い時間
    const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

    // 現在時刻を取得
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    // 前回基準からの経過時間を取得
    std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

    // 1/60秒経っていない場合
    if (elapsed < kMinCheckTime) {
        // 1/60秒経過するまで微小なスリープを繰り返す
        while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
            std::this_thread::sleep_for(std::chrono::microseconds(1)); // 1マイクロ秒スリープ
        }
    }

    // 現在時刻を基準時間として記録
    reference_ = std::chrono::steady_clock::now();
}


void Frame::SetTimeScale(float scale) {
    if (scale < 0.0f) {
        scale = 0.0f; // 負の値を防ぐ
    }
    timeScale_ = scale;
}

float Frame::GetTimeScale() {
    return timeScale_;
}