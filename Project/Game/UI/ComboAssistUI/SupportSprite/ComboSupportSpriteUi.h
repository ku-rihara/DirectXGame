#pragma once
// Engine
#include "2D/Sprite.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
// std
#include <array>
#include <cstdint>
#include <memory>
#include <string>

struct GameObj;
class ComboAssistController;

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

    void Connect(GameObj* go);
    void SetComboAssistController(ComboAssistController* controller) { pComboAssist_ = controller; }

private:
    void RegisterParams();
    void UpdateBgPositions();
  
private:
    ComboAssistController*       pComboAssist_     = nullptr;
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_                 = "ComboSupportSpriteUi";
    
    // 背景スプライト×2
    std::array<std::unique_ptr<KetaEngine::Sprite>, static_cast<int32_t>(BgType::COUNT)> bgSprites_;

    // 発動条件切替スプライト
    std::unique_ptr<KetaEngine::Sprite> conditionSprite_;

    // 各スプライトのオフセット
    std::array<Vector2, static_cast<int32_t>(BgType::COUNT)> bgOffsets_;
    Vector2 conditionOffset_;

    // テクスチャパス
    const std::string kBgTextures[static_cast<int32_t>(BgType::COUNT)] = {
        "ComboSupportUI/ComboSupportBg.dds",
        "ComboSupportUI/ComboSupportBg.dds",
    };
   const std::string kConditionTexture = "ComboSupportUI/ComboCondition.dds";
};
