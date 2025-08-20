#pragma once
#include "CameraAnimationData.h"
#include <memory>
#include <string>
#include <vector>

class CameraEditor {
public:
    CameraEditor()  = default;
    ~CameraEditor() = default;

    void AllLoadFile();
    void AllSaveFile();
    void AddAnimation(const std::string& animationName);
    void Update(float deltaTime);
    void EditorUpdate();
    CameraAnimationData* GetSelectedAnimation();

private:
    std::vector<std::unique_ptr<CameraAnimationData>> animations_;
    int selectedIndex_ = -1;

    // 入力用バッファ
    char nameBuffer_[128] = "";
};
