#pragma once
#include "2d/Sprite.h"
#include "Vector2.h"
#include <memory>

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
    // 初期化、更新
    virtual void Init();
    virtual void Update();

    // 位置設定
    void SetPosition(const Vector2& pos);

    // スケール設定（均一スケール）
    void SetScale(float scale);

    // Z軸回転設定（ラジアン）
    void SetRotation(float rotZ);

protected:
    // UI
    std::unique_ptr<KetaEngine::Sprite> uiSprite_;

    // ロックオーバーレイ
    std::unique_ptr<KetaEngine::Sprite> outLineUI_;

    // 発動中・発動済みの攻撃アウトライン
    std::unique_ptr<KetaEngine::Sprite> activeOutLineUI_;

    // アシストのステート
    AsistState state_ = AsistState::NONE;

public:
    // getter
    AsistState GetState() const { return state_; }

    // setter
    void SetState(AsistState state) { state_ = state; }

    // 攻撃発動アウトラインの表示/非表示
    void SetActiveOutLine(bool visible);

    // プッシュスケーリングアニメーション再生
    void PlayPushScaling();
};
