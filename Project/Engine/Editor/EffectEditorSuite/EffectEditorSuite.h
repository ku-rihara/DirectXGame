#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <memory>

class ObjEaseAnimationEditor;
class CameraEditor;
class EffectEditorSuite {
public:
    // Effect Editorの種類
    enum class EffectEditorType {
        ObjEaseAnimation,
        Count
    };

public:
    EffectEditorSuite()  = default;
    ~EffectEditorSuite() = default;
    //*----------------------------- public Methods -----------------------------*//

    // 初期化、更新、エディター更新
    void Init();
    void Update(const float& deltaTime);
    void EditorUpdate();

    // 共通操作
    void AddEffect(const std::string& name);
    void RemoveEffect(const int32_t& index);

    // セーブ、ロード
    virtual void AllLoadFile();
    virtual void AllSaveFile();

private:
    std::unique_ptr<ObjEaseAnimationEditor> objEaseAnimationEditor_;
    std::unique_ptr<CameraEditor> cameraEditor_;
};