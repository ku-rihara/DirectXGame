#pragma once

#include "EasingCreator.h"
#include "Vector2.h"
#include "Vector3.h"
#include <filesystem>
#include <string>

class EasingEditor {
public:
    EasingEditor()  = default;
    ~EasingEditor() = default;

    void Init();
    void Edit();
    void LoadAll();
    void SaveAll();

private:

    enum class TabType {
        Float,
        Vector2,
        Vector3
    };

private:
    const std ::string kDirectoryPath_ = "Resources/EasingParameter/";
    TabType currentTab_                = TabType::Float;

    std ::string floatPath_;
    std ::string vec2Path_;
    std ::string vec3Path_;

    EasingCreator<float>   fEasingCreator_;
    EasingCreator<Vector2> vec2EasingCreator_;
    EasingCreator<Vector3> vec3EasingCreator_;
};