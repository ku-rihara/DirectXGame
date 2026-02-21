#pragma once
#include "2d/Sprite.h"
#include <memory>

/// <summary>
/// コンボアシストのサポートスプライト基底クラス
/// </summary>
class BaseSupportSpriteUI {
public:
    BaseSupportSpriteUI()          = default;
    virtual ~BaseSupportSpriteUI() = default;

    virtual void Init() {}
    virtual void Update() {}

protected:
    std::unique_ptr<KetaEngine::Sprite> uiSprite_;
};
