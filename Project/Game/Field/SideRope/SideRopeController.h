#pragma once

#include "SideRope.h"
#include "RopeBoundParam.h"
#include <array>
#include <memory>

/// <summary>
/// サイドロープの制御クラス
/// </summary>
class SideRopeController {
public:
public:
    SideRopeController()  = default;
    ~SideRopeController() = default;

    // 初期化、更新
    void Init();
    void Update();

    void AdjustParam(); //< パラメータ調整
private:
    // パラメータ登録
    void RegisterParams();
private:
    // global parameter
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "RopeController";

    // parameter
    BounceParam ropeParam_;

    std::array<std::unique_ptr<SideRope>, static_cast<int32_t>(SideRopeType::COUNT)> sideRopes_;
};