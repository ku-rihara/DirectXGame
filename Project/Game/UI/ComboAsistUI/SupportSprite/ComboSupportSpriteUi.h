#pragma once
// Engine
#include "2d/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

class ComboAsistController;

/// <summary>
/// コンボアシストUIのサポートスプライトUI
/// 背景スプライト×2と、発動条件に応じてUVが切り替わるスプライト×1を管理する
///
/// 【条件スプライトのテクスチャ連番（横並び）】
///   [ GROUND(地上) | AIR(空中) ]
///   uvScale.x = 0.5f
///   GROUND : uvPosX = 0.0f
///   AIR    : uvPosX = 0.5f
/// </summary>
class ComboSupportSpriteUi {
public:
    enum class BgType {
        BG1,
        BG2,
        COUNT,
    };

    ComboSupportSpriteUi()  = default;
    ~ComboSupportSpriteUi() = default;

    void Init();
    void Update();
    void AdjustParam();

    /// ゲームイントロに合わせてスケールを外部から制御する
    void SetScale(const Vector2& scale);

    void SetComboAsistController(ComboAsistController* controller) { pComboAsist_ = controller; }

private:
    void RegisterParams();
    void UpdateBgPositions();
    void UpdateConditionSprite();

private:
    ComboAsistController*       pComboAsist_     = nullptr;
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                 = "ComboSupportSpriteUi";
    
    // 背景スプライト×2
    std::array<std::unique_ptr<KetaEngine::Sprite>, static_cast<int32_t>(BgType::COUNT)> bgSprites_;

    // 発動条件切替スプライト（UV横並び: GROUND左 | AIR右）
    std::unique_ptr<KetaEngine::Sprite> conditionSprite_;

    // 各スプライトのオフセット（basePositionからの相対位置）
    std::array<Vector2, static_cast<int32_t>(BgType::COUNT)> bgOffsets_;
    Vector2 conditionOffset_;

    // テクスチャパス
    const std::string kBgTextures[static_cast<int32_t>(BgType::COUNT)] = {
        "ComboSupportUI/ComboSupportBg.dds",
        "ComboSupportUI/ComboSupportBg.dds",
    };
    static constexpr const char* kConditionTexture = "ComboSupportUI/ComboCondition.dds";
};
