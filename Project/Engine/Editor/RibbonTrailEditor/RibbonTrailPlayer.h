#pragma once
#include "RibbonTrailData.h"
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <memory>
#include <string>

namespace KetaEngine {

class RibbonTrail;

/// <summary>
/// リボントレイル再生クラス
/// </summary>
class RibbonTrailPlayer : public BaseEffectPlayer {
public:
    RibbonTrailPlayer()           = default;
    ~RibbonTrailPlayer() override = default;

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    /// <summary>
    /// 指定プリセットをロードして再生状態にする
    /// </summary>
    void Play(const std::string& presetName, const std::string& categoryName = "Common") override;

    /// <summary>
    /// Follow モード用：追従するワールド座標ポインタを設定
    /// Updateで自動 Emit される
    /// </summary>
    void SetFollowPosition(const Vector3* pos) { followPosition_ = pos; }

    /// <summary>
    /// 弧モード用：弧の中心となる基準座標を設定する
    /// Play() の前後どちらで呼んでも有効。
    /// nullptr を渡すとワールド原点を基準にする。
    /// </summary>
    void SetBasePos(const Vector3* basePos) { basePos_ = basePos; }

    /// <summary>
    /// 弧モード用：基準座標を Vector3 の値で直接指定する
    /// （ポインタではなく値渡しが必要な場合）
    /// </summary>
    void SetBasePosValue(const Vector3& basePos) {
        basePosValue_ = basePos;
        basePos_      = &basePosValue_;
    }

    /// <summary>
    /// 弧モードで即座に弧を生成する（Arcモード時にPlayと合わせて呼ぶ）
    /// Play() の後に呼ぶこと。
    /// </summary>
    void EmitArc();

    /// <summary>
    /// エミットを停止するが既存ポイントはLifeTimeまで残す
    /// </summary>
    void StopEmit();

    /// <summary>
    /// トレイルをクリアして非表示にする
    /// </summary>
    void StopAndClear();

    bool IsActive() const { return isActive_; }

protected:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    RibbonTrailData* GetData() const;

    void SyncDataToTrail();

    /// <summary>
    /// 弧データから RibbonTrail::SetArc を呼び出す内部処理
    /// </summary>
    void ApplyArcToTrail();

private:
    RibbonTrail*   trail_          = nullptr;
    float          emitTimer_      = 0.0f;
    bool           isActive_       = false;

    // Follow モード用
    const Vector3* followPosition_ = nullptr;

    // Arc / 基準座標用
    const Vector3* basePos_        = nullptr;
    Vector3        basePosValue_   = {0.0f, 0.0f, 0.0f}; ///< SetBasePosValue の値保持

public:
    ///========================================================
    /// Getter
    ///========================================================
    Vector4            GetStartColor()              const;
    Vector4            GetEndColor()                const;
    float              GetStartWidth()              const;
    float              GetEndWidth()                const;
    float              GetLifetime()                const;
    int32_t            GetMaxPoints()               const;
    float              GetEmitInterval()            const;
    const std::string& GetTexturePath()             const;
    bool               GetUseDistortion()           const;
    float              GetDistortionStrength()      const;
    const std::string& GetDistortionTexturePath()   const;
    Vector2            GetUVScrollSpeed()           const;
    Vector2            GetDistortionUVScrollSpeed() const;
};

} // KetaEngine
