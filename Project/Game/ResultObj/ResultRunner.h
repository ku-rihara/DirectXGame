#pragma once

#include "3d/AnimationObject3D/Object3DAnimation.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// リザルト画面のランナーキャラクター
/// 複数の敵が正面に向かって走り続けるループ演出
/// </summary>
class ResultRunner {
public:
    ResultRunner()  = default;
    ~ResultRunner() = default;

    void Init();
    void Update();
    void AdjustParam();

private:
    void RegisterParams();

    struct Config {
        Vector3 basePos    = {0.f, 0.f, 0.f};   ///< 全体の基準位置（XY）
        float   zSpacing   = 3.f;                ///< ランナー間のZ間隔
        float   runSpeed   = 5.f;                ///< 走る速度（毎秒Z減少量）
        float   loopStartZ = 15.f;               ///< ループ折り返し開始Z位置（遠方）
        float   loopEndZ   = -5.f;               ///< ループ境界Z位置（カメラ手前）
        float   rotateY    = 3.14159f;           ///< Y軸回転（正面向き）
        Vector3 scale      = {1.f, 1.f, 1.f};   ///< スケール
    };

    static constexpr int32_t kRunnerCount = 3;

    Config config_;
    std::array<float, kRunnerCount> currentZ_ = {};

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "ResultRunner";

    std::vector<std::unique_ptr<KetaEngine::Object3DAnimation>> runnerAnimations_;
};
