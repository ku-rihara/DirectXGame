#pragma once
#include <functional>
#include <string>

namespace KetaEngine {

/// <summary>
/// EffectEditorSuiteのインライン編集ウィンドウを開くためのリクエスト仲介クラス
/// </summary>
class EffectInlineEditRequest {
public:
    using CallbackFunc = std::function<void(const std::string& editorType, const std::string& name, const std::string& category)>;

    static void SetCallback(CallbackFunc cb);
    static void Request(const std::string& editorType, const std::string& name, const std::string& category);

private:
    static CallbackFunc callback_;
};

} // namespace KetaEngine
