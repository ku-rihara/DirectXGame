#include "CameraAnimationData.h"
#include <algorithm>

CameraAnimationData::CameraAnimationData(const std::string& name)
    : name_(name) {}

void CameraAnimationData::AddKeyFrame(const CameraKeyFrame& keyFrame) {
    keyFrames_.push_back(keyFrame);
    RecalculateTotalDuration();
}

void CameraAnimationData::RemoveKeyFrame(size_t index) {
    if (index < keyFrames_.size()) {
        keyFrames_.erase(keyFrames_.begin() + index);
        RecalculateTotalDuration();
    }
}

const std::vector<CameraKeyFrame>& CameraAnimationData::GetKeyFrames() const {
    return keyFrames_;
}

std::vector<CameraKeyFrame>& CameraAnimationData::GetKeyFrames() {
    return keyFrames_;
}

const std::string& CameraAnimationData::GetName() const {
    return name_;
}

void CameraAnimationData::SetName(const std::string& name) {
    name_ = name;
}

float CameraAnimationData::GetTotalDuration() const {
    return totalDuration_;
}

void CameraAnimationData::SortKeyFramesByTime() {
    std::sort(keyFrames_.begin(), keyFrames_.end(),
        [](const CameraKeyFrame& a, const CameraKeyFrame& b) {
            return a.timePoint < b.timePoint;
        });
    RecalculateTotalDuration();
}

void CameraAnimationData::RecalculateTotalDuration() {
    if (!keyFrames_.empty()) {
        totalDuration_ = keyFrames_.back().timePoint;
    } else {
        totalDuration_ = 0.0f;
    }