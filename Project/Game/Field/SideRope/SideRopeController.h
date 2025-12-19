#pragma once

#include"SideRope.h"
#include <array>
#include <memory>

/// <summary>
/// サイドロープの制御クラス
/// </summary>
class SideRopeController {
public:
  
    SideRopeController() = default;
    ~SideRopeController() = default;

    // 初期化、更新
    void Init();
    void Update();

    void AdjustParam(); //< パラメータ調整

private:
    std::array<std::unique_ptr<SideRope>, static_cast<int32_t>(SideRopeType::COUNT)> sideRopes_;
};