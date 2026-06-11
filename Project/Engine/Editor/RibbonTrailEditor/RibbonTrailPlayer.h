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

// リボントレイル再生クラス
class RibbonTrailPlayer : public BaseEffectPlayer {
public:
    RibbonTrailPlayer()           = default;
    ~RibbonTrailPlayer() override = default;

    // 初期化、更新
    void Init() override;
    void Update(float speedRate = 1.0f) override;

    // 再生
    void Play(const std::string& presetName, const std::string& categoryName = "Common") override;

    // 弧モードで即座に弧を生成
    void EmitArc();

    // エミット停止、クリア
    void StopEmit();
    void StopAndClear();

    bool IsActive() const { return isActive_; }

protected:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    RibbonTrailData* GetData() const;

    void SyncDataToTrail();
    void ApplyArcToTrail();

private:
    RibbonTrail*   trail_          = nullptr;
    float          emitTimer_      = 0.0f;
    bool           isActive_       = false;

    // Follow モード用
    const Vector3* followPosition_ = nullptr;

    // 基準座標用
    const Vector3* basePos_        = nullptr;
    Vector3        basePosValue_   = {0.0f, 0.0f, 0.0f};

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

    ///========================================================
    /// Setter
    ///========================================================
    void SetFollowPosition(const Vector3* pos) { followPosition_ = pos; }
    void SetBasePos(const Vector3* basePos) { basePos_ = basePos; }
    void SetBasePosValue(const Vector3& basePos) {
        basePosValue_ = basePos;
        basePos_      = &basePosValue_;
    }
};

} // KetaEngine
