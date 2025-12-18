#pragma once

#include"SideRope.h"
#include <array>
#include <memory>

/// <summary>
/// サイドロープの制御クラス
/// </summary>
class SideRopeController {
public:
    enum class SidePos {
        FRONT,
        LEFT,
        RIGHT,
        BACK,
        COUNT,
    };

public:
  
    SideRopeController() = default;
    ~SideRopeController() = default;

    // 初期化、更新
    void Init();
    void Update();

private:
    std::array<std::unique_ptr<SideRope>, static_cast<size_t>(SidePos::COUNT)> sideRopes_;
};