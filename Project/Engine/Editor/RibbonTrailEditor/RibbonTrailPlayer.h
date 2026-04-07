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

class RibbonTrail; // 前方宣言

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
    /// フォロー位置を設定（Updateで自動Emit）
    /// </summary>
    void SetFollowPosition(const Vector3* pos) { followPosition_ = pos; }

    /// <summary>
    /// トレイルをクリアして非表示にする
    /// </summary>
    void StopAndClear();

    bool IsActive() const { return isActive_; }

protected:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    RibbonTrailData* GetData() const;

    /// <summary>
    /// Play() 時と、エディタでパラメータ変更時に呼ぶ
    /// </summary>
    void SyncDataToTrail();

private:
    RibbonTrail*     trail_          = nullptr;
    float            emitTimer_      = 0.0f;
    bool             isActive_       = false;
    const Vector3*   followPosition_ = nullptr;

public:
    ///========================================================
    /// Getter
    ///========================================================
    Vector4            GetStartColor()             const;
    Vector4            GetEndColor()               const;
    float              GetStartWidth()             const;
    float              GetEndWidth()               const;
    float              GetLifetime()               const;
    int32_t            GetMaxPoints()              const;
    float              GetEmitInterval()           const;
    const std::string& GetTexturePath()            const;
    bool               GetUseDistortion()         const;
    float              GetDistortionStrength()     const;
    const std::string& GetDistortionTexturePath() const;
    Vector2            GetUVScrollSpeed()              const;
    Vector2            GetDistortionUVScrollSpeed()    const;
};

} // KetaEngine
