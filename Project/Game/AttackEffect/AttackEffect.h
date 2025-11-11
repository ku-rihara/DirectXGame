#pragma once
#include "Editor/PostEffectEditor/PostEffectController.h"
#include "Editor/TimeScaleEditor/TimeScaleController.h"
#include <memory>

/// <summary>
/// 攻撃時のエフェクト管理クラス
/// </summary>
class AttackEffect {
public:
    AttackEffect()  = default;
    ~AttackEffect() = default;

    void Init();         //< 初期化
    void Update();       //< 更新
    void EditorUpdate(); //< エディタ更新

    /// <summary>
    /// ヒットストップの再生
    /// </summary>
    /// <param name="timeScaleName">タイムスケール名</param>
    void PlayHitStop(const std::string& timeScaleName);

    /// <summary>
    /// ポストエフェクトの再生
    /// </summary>
    /// <param name="effectName">エフェクト名</param>
    void PlayPostEffect(const std::string& effectName);

private:
    std::unique_ptr<TimeScaleController> timeScaleController_;
    std::unique_ptr<PostEffectController> postEffectController_;
};