#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include "math/MathFunction.h"
#include "Frame/Frame.h"
#include <cstdint>
#include <memory>

/// <summary>
/// レイアウト情報
/// </summary>
struct LayoutParam {
    Vector2 basePosition;
    Vector2 arrowOffset;
    float columnSpacing;
    float rowSpacing;
    float branchYOffset;
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

    // 位置設定
    void SetPosition(const Vector2& pos);

    // スケール設定（均一スケール）
    void SetScale(float scale);

    // Z軸回転設定（ラジアン）
    void SetRotation(float rotZ);

    // 表示/非表示
    virtual void SetVisible(bool visible);

protected:
    // UI
    std::unique_ptr<KetaEngine::Sprite> uiSprite_;

    // ロックオーバーレイ
    std::unique_ptr<KetaEngine::Sprite> outLineUI_;

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
    float lerpSpeed_ = 10.0f;
    bool needsLerpUpdate_ = false;

public:
    // getter
    AsistState GetState() const { return state_; }
    int32_t GetRowNum() const { return rowNum_; }
    int32_t GetColumnNum() const { return columnNum_; }

    // setter
    void SetState(AsistState state) { state_ = state; }
    void SetRowColumn(int32_t row, int32_t column);

    // 攻撃発動アウトラインの表示/非表示
    void SetActiveOutLine(bool visible);

    // Lerpせず即座にターゲット位置へスナップ
    void SnapToTarget();

    // プッシュスケーリングアニメーション再生
    void PlayPushScaling();
};
