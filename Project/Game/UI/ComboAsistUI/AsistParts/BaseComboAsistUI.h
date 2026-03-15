#pragma once
#include "2d/Sprite.h"
#include "Frame/Frame.h"
#include "math/MathFunction.h"
#include "Vector2.h"
#include <cstdint>
#include <memory>
#include <string>

/// <summary>
/// レイアウト情報
/// </summary>
struct LayoutParam {
    Vector2 basePosition;
    Vector2 arrowOffset;
    float columnSpacing;
    float rowSpacing;
    float yGroupOffsetY; 
    float buttonScale;
    float arrowScale;
};

/// <summary>
/// コンボアシストで使用するUIの基底クラス
/// </summary>
class BaseComboAsistUI {
public:
    BaseComboAsistUI()          = default;
    virtual ~BaseComboAsistUI() = default;

    enum class AsistState {
        NONE,
        PLAYING,
        NEXTATTACK,
    };

public:
    // 更新
    virtual void Update();

    // レイアウト情報から位置・スケールを計算して反映する
    virtual void ApplyLayout() {}

    // スライドオフセットを加味して再配置
    void ApplySlideOffset(float offsetX);

    //
    void SetPosition(const Vector2& pos);
    void SetScale(const Vector2& scale);

    void SetExtraScale(float extraScale);
    void SetRotation(float rotZ);
    void SetTargetPosY(float y);

    // 表示/非表示（即時）
    virtual void SetVisible(bool visible);

    // 範囲内外の遷移時にスケールアニメーション付きで表示切替
    void SetRangeVisible(bool inRange);

protected:
    // UI
    std::unique_ptr<KetaEngine::Sprite> uiSprite_;

    // ロックオーバーレイ
    std::unique_ptr<KetaEngine::Sprite> lockUI_;

    // 発動中・発動済みの攻撃アウトライン
    std::unique_ptr<KetaEngine::Sprite> activeOutLineUI_;

    // アシストのステート
    AsistState state_ = AsistState::NONE;

    int32_t rowNum_    = 0;
    int32_t columnNum_ = 0;

    // レイアウト情報
    LayoutParam layout_ = {};

    // スライドオフセット
    float slideOffsetX_ = 0.0f;

    // Lerp位置補間
    Vector2 currentDisplayPos_;
    Vector2 targetPos_;
    float lerpSpeed_      = 10.0f;
    bool needsLerpUpdate_ = false;

    Vector2 baseScale_ = {1.0f, 1.0f};
    float extraScale_  = 1.0f;

    // 範囲内外のアニメーション管理
    bool isInRange_         = false;
    bool isScaleOutPlaying_ = false;

public:
    // getter
    AsistState GetState() const { return state_; }
    Vector2 GetBaseScale() const { return baseScale_; }
    int32_t GetRowNum() const { return rowNum_; }
    int32_t GetColumnNum() const { return columnNum_; }

    // setter
    void SetState(AsistState state) { state_ = state; }
    void SetRowColumn(int32_t row, int32_t column);

    // 攻撃発動アウトラインの表示/非表示
    void SetActiveOutLine(bool visible);

    // Lerpせず即座にターゲット位置へスナップ
    void SnapToTarget();

    // アニメーションなしで範囲状態を即時設定（コンボリセット後など）
    void SnapRangeState(bool inRange);

    // プッシュスケーリングアニメーション再生
    void PlayPushScaling();

protected:
    // スケールアニメーション（サブクラスでオーバーライド可）
    virtual void PlayScaleIn();
    virtual void PlayScaleOut();
};