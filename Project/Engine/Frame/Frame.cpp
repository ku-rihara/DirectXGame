// Time.cpp
#include "Frame.h"


std::chrono::high_resolution_clock::time_point Frame::lastTime_ = std::chrono::high_resolution_clock::now();
float Frame::deltaTime_ = 0.0f;

void Frame::Init() {
    lastTime_ = std::chrono::high_resolution_clock::now();
}

void Frame::Update() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime_;
    deltaTime_ = elapsed.count(); // 秒単位の経過時間
    lastTime_ = currentTime; // 次の更新のために現在の時間を記録
}

float Frame::DeltaTime() {
    return deltaTime_; // 前回の更新からの経過時間を返す
}
