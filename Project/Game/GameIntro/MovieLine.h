#pragma once

#include "2d/Sprite.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include"Easing/Easing.h"
#include <array>
#include <memory>
#include <string>

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

    void Init();
    void Update();
    void Draw();

    // editor
    void AdjustParam();
    void BindParams();

private:
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "MovieLine";

    std::array<std::unique_ptr<Sprite>, 2> sprite_;
    std::array<Vector2, 2> initPosition_;
    std::array<Easing<float>, 2> easing_;
};