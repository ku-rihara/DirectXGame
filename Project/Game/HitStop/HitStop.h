#pragma once
#include"utility/TimeScaleEditor/TimeScaleController.h"
#include <memory>

class HitStop {
public:
    HitStop()=default;
    ~HitStop() = default;

    void Init();
    void Update();
    void PlayHitStop(const std::string& timeScaleName);

    void EditorUpdate();

private:
    std::unique_ptr<TimeScaleController> timeScaleController_;

};