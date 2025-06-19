#pragma once

#include "Easing.h"
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

    template <typename T>
    void SetAutoApplyTarget(Easing<T>* easing, const std::string& presetName);
    void UpdatePreview();

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

      std::string autoPresetName_;

    EasingCreator<float> fEasingCreator_;
    EasingCreator<Vector2> vec2EasingCreator_;
    EasingCreator<Vector3> vec3EasingCreator_;

    Easing<float>* fTarget_    = nullptr;
    Easing<Vector2>* v2Target_ = nullptr;
    Easing<Vector3>* v3Target_ = nullptr;
};