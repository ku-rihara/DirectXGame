#pragma once

#include "2d/Sprite.h"
#include "Vector2.h"
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// 数字1桁スプライト（GlobalParameter不使用・動的生成向け）
/// </summary>
class NumberDigitUI {
public:
    NumberDigitUI()  = default;
    ~NumberDigitUI() = default;

    void Init(const std::string& name = "");
    void Update(int32_t digit, const Vector2& pos, const Vector2& scale, float alpha, bool isVisible = true);

private:
    static constexpr float kUVStep = 0.1f;

    std::unique_ptr<KetaEngine::Sprite> sprite_;
};
