#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include <memory>
#include <string>

/// <summary>
/// ポストエフェクト再生クラス
/// </summary>
namespace KetaEngine {

class PostEffectData;

class PostEffectPlayer : public BaseEffectPlayer {
public:
    PostEffectPlayer()           = default;
    ~PostEffectPlayer() override = default;

    void Init() override;
    void Update(float speedRate = 1.0f) override;

    /// <summary>
    /// 指定プリセットをロードして再生する
    /// </summary>
    void Play(const std::string& presetName, const std::string& categoryName = "Common") override;

    /// <summary>
    /// エフェクトを即時停止してレンダラーをNONEにリセット
    /// </summary>
    void StopAndReset();

protected:
    std::unique_ptr<BaseEffectData> CreateEffectData() override;

private:
    PostEffectData* GetData() const;
};

} // namespace KetaEngine
