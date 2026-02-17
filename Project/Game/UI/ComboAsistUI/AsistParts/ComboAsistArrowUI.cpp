#include "ComboAsistArrowUI.h"
#include <cmath>

void ComboAsistArrowUI::Init(int32_t fromCol, int32_t fromRow, int32_t toCol, int32_t toRow, const LayoutParam& layout) {
    fromCol_ = fromCol;
    fromRow_ = fromRow;
    toCol_   = toCol;
    toRow_   = toRow;
    layout_  = layout;

    uiSprite_.reset(KetaEngine::Sprite::Create("OperateUI/Arrow.dds", false));

    const int32_t layerNum = 20; // レイヤー番号
    if (uiSprite_) {
        uiSprite_->SetLayerNum(layerNum);
        uiSprite_->SetAnchorPoint({0.5f, 0.5f});
    }

    // レイアウト適用
    ApplyLayout();

    // 初期配置はスナップ（Lerpなしで即座に反映）
    currentDisplayPos_ = targetPos_;
    SetPosition(currentDisplayPos_);
    needsLerpUpdate_ = false;
}

void ComboAsistArrowUI::Update() {
    BaseComboAsistUI::Update();
}

void ComboAsistArrowUI::ApplyLayout() {
    // 始点・終点のボタン位置を計算
    Vector2 fromPos(
        layout_.basePosition.x + fromCol_ * layout_.columnSpacing + slideOffsetX_,
        layout_.basePosition.y + fromRow_ * (layout_.rowSpacing + layout_.branchYOffset));
    Vector2 toPos(
        layout_.basePosition.x + toCol_ * layout_.columnSpacing + slideOffsetX_,
        layout_.basePosition.y + toRow_ * (layout_.rowSpacing + layout_.branchYOffset));

    // 矢印位置：始点からオフセット + 縦方向は中間
    Vector2 arrowPos(
        fromPos.x + layout_.arrowOffset.x,
        fromPos.y + layout_.arrowOffset.y + (toPos.y - fromPos.y) * 0.5f);

    // ターゲット位置を設定（Lerpで補間される）
    targetPos_ = arrowPos;
    needsLerpUpdate_ = true;

    // 回転角度
    float dx = toPos.x - fromPos.x;
    float dy = toPos.y - fromPos.y;
    SetRotation(std::atan2(dy, dx));

    SetScale(layout_.arrowScale);
}

void ComboAsistArrowUI::ShiftColumns(int32_t delta) {
    fromCol_ += delta;
    toCol_ += delta;
}
