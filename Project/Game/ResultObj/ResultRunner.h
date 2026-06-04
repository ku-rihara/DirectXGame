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
        Vector3 basePos; ///< 全体の基準位置
        float zSpacing; ///< ランナー間のZ間隔
        float runSpeed; ///< 走る速度
        float loopStartZ; ///< ループ折り返し開始Z位置
        float loopEndZ; ///< ループ境界Z位置
        float rotateY; ///< Y軸回転
        Vector3 scale; ///< スケール
    };

private:
    static constexpr int32_t kRunnerCount = 3;

    Config config_;
    std::array<float, kRunnerCount> currentZ_ = {};

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "ResultRunner";

    std::vector<std::unique_ptr<KetaEngine::Object3DAnimation>> runnerAnimations_;
};
