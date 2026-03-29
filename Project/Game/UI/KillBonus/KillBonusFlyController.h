#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "KillBonusFlyEntry.h"
#include "Vector2.h"
#include "Vector3.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace KetaEngine {
class ViewProjection;
} // namespace KetaEngine

/// <summary>
/// 撃破時にスプライトをベジエ曲線で飛ばし、終点到達時にコールバックを呼ぶクラス
///
/// </summary>
class KillBonusFlyController {
public:
    KillBonusFlyController()  = default;
    ~KillBonusFlyController() = default;

    void Init();
    void Update(float deltaTime);
    void AdjustParam();

    /// 敵死亡時に EnemyManager から呼ぶ。pending の comboBonusValue を消費してエントリを生成
    void SpawnFromPending(const Vector3& worldPos, const KetaEngine::ViewProjection& vp);

private:
    void RegisterParams();

private:
    std::vector<std::unique_ptr<KillBonusFlyEntry>> entries_;

    float pendingComboBonusValue_ = 1.0f;

    // GlobalParameter で調整可能なパラメータ
    Vector2 endPos_;
    Vector2 controlOffset_;
    float   controlXOffsetRange_;
    Vector2 controlOffset2_;
    float   controlXOffsetRange2_;
    float duration_;
    Vector2 spriteScale_;

    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                  = "KillBonusFlyUI";

    std::function<void(float)> onReachCallback_;
    std::function<void()>      onGaugeAnimCallback_;

public:
    ///Setter
    void SetPendingComboBonusValue(float value) { pendingComboBonusValue_ = value; }
    void SetOnReachCallback(std::function<void(float)> cb) { onReachCallback_ = std::move(cb); }
    void SetOnGaugeAnimCallback(std::function<void()> cb)  { onGaugeAnimCallback_ = std::move(cb); }
};
