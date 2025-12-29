#pragma once
#include "2d/Sprite.h"
#include "Easing/Easing.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include <array>
#include <memory>
#include <string>

/// <summary>
/// ムービーライン表示クラス
/// </summary>
class MovieLine {
public:
    enum class Type {
        UP,
        DOWN,
        COUNT,
    };

public:
    MovieLine()  = default;
    ~MovieLine() = default;

    // 初期化、更新
    void Init();
    void AppearUpdate(); //< 出現更新
    void ExitUpdate(); //< 退出更新

    // editor
    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータバインド

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "MovieLine";
    std::array<std::unique_ptr<KetaEngine::Sprite>, 2> sprite_;
    std::array<Vector2, 2> appearPosition_;
    std::array<Vector2, 2> appearAdaptPos_;
    std::array<Vector2, 2> exitAdaptPos_;
    std::array<KetaEngine::Easing<float>, 2> exitEasing_;
    std::array<KetaEngine::Easing<float>, 2> appearEasing_;
};