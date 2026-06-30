#include "EffectInlineEditRequest.h"

using namespace KetaEngine;

EffectInlineEditRequest::CallbackFunc EffectInlineEditRequest::callback_;

void EffectInlineEditRequest::SetCallback(const CallbackFunc& cb) {
    callback_ = cb;
}

void EffectInlineEditRequest::Request(const std::string& editorType, const std::string& name, const std::string& category) {
    if (callback_) {
        callback_(editorType, name, category);
    }
}
