#pragma once
#include "Easing/Easing.h"
#include "EasingCreator.h"
#include "Vector2.h"
#include "Vector3.h"
#include <filesystem>
#include <string>

/// <summary>
/// イージングエディタクラス
/// </summary>
class EasingEditor {
public:
    EasingEditor()  = default;
    ~EasingEditor() = default;

    void Init(); //< 初期化
    void Edit(); //< 編集
    void LoadAll(); //< 全読み込み
    void SaveAll(); //< 全保存

private:
    enum class TabType {
        Float,
        Vector2,
        Vector3
    };

private:
    const std::string kDirectoryPath_ = "Resources/EasingParameter/";
    TabType currentTab_               = TabType::Float;
    std::string floatPath_;
    std::string vec2Path_;
    std::string vec3Path_;
    std::string autoPresetName_;
    EasingCreator<float> fEasingCreator_;
    EasingCreator<Vector2> vec2EasingCreator_;
    EasingCreator<Vector3> vec3EasingCreator_;
    Easing<float>* fTarget_    = nullptr;
    Easing<Vector2>* v2Target_ = nullptr;
    Easing<Vector3>* v3Target_ = nullptr;

public:
    void SetFloatTarget(Easing<float>* target) { fTarget_ = target; }
    void SetVector2Target(Easing<Vector2>* target) { v2Target_ = target; }
    void SetVector3Target(Easing<Vector3>* target) { v3Target_ = target; }
};