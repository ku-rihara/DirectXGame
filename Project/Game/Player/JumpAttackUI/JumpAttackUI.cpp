#include "JumpAttackUI.h"
#include "Frame/Frame.h"
#include <imgui.h>

void JumpAttackUI::Init() {

    // スプライト生成
    bottoms_[X].sprite.reset(Sprite::Create("OperateUI/OperateX.png", false));
    bottoms_[Y].sprite.reset(Sprite::Create("OperateUI/OperateY.png", false));
    bottoms_[B].sprite.reset(Sprite::Create("OperateUI/OperateY.png", false));
    bottoms_[A].sprite.reset(Sprite::Create("OperateUI/OperateY.png", false));

    // 移動向き決定
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        DirectionByBottom(i);
    }

    // Global parameter
    globalParameter_ = GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_, false);
    BindParams();
    globalParameter_->SyncParamForGroup(groupName_);

    EasingInit();
}

void JumpAttackUI::Update(const Vector3& basePos, const ViewProjection& viewProjection) {
    (this->*spFuncTable_[static_cast<size_t>(state_)])();

    // ターゲットの座標取得
    Vector3 positionWorld = basePos;

    // ワールド座標からスクリーン座標に変換
    Vector2 positionScreen = ScreenTransform(positionWorld, viewProjection);

    // Vector2に格納
    Vector2 positionScreenV2 = positionScreen;

    // スプライト適応
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        bottoms_[i].sprite->transform_.pos      = bottoms_[i].discreteDirection * easingParam_.posEaseValue + positionScreenV2;
        bottoms_[i].sprite->transform_.scale    = easingParam_.scaleEaseValue;
        bottoms_[i].sprite->transform_.rotate.z = easingParam_.rotateEaseValue;
    }
}
void JumpAttackUI::DirectionByBottom(const size_t& TypeNum) {

    switch (TypeNum) {
    case X:
        bottoms_[X].discreteDirection = Vector2(-1.0f, 0.0f);
        break;
    case Y:
        bottoms_[Y].discreteDirection = Vector2(0.0f, 1.0f);
        break;
    case B:
        bottoms_[B].discreteDirection = Vector2(1.0f, 0.0f);
        break;
    case A:
        bottoms_[A].discreteDirection = Vector2(0.0f, -1.0f);
        break;
    default:
        break;
    }
}

void JumpAttackUI::Wait() {
}
void JumpAttackUI::Open() {

    easingParam_.scaleEasing->Update(Frame::DeltaTime());
    easingParam_.posEasing->Update(Frame::DeltaTime());
    easingParam_.rotateEasing->Update(Frame::DeltaTime());
}
void JumpAttackUI::Close() {
    easingParam_.scaleEasing->Update(-Frame::DeltaTime());
    easingParam_.posEasing->Update(-Frame::DeltaTime());
    easingParam_.rotateEasing->Update(-Frame::DeltaTime());
}

void JumpAttackUI::BindParams() {
}

void JumpAttackUI::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void JumpAttackUI::EasingInit() {
    easingParam_.scaleEasing  = std::make_unique<Easing<Vector2>>();
    easingParam_.posEasing    = std::make_unique<Easing<float>>();
    easingParam_.rotateEasing = std::make_unique<Easing<float>>();

    // Init
    easingParam_.scaleEasing->Init("PurposeCloseScaleY", "PurposeCloseScaleY.json");
    easingParam_.posEasing->Init("PurposeCloseScaleY", "PurposeCloseScaleY.json");
    easingParam_.rotateEasing->Init("PurposeCloseScaleY", "PurposeCloseScaleY.json");

    // scale
    easingParam_.scaleEasing->SetAdaptValue(&easingParam_.scaleEaseValue);
    easingParam_.scaleEasing->Reset();

    // pos
    easingParam_.posEasing->SetAdaptValue(&easingParam_.posEaseValue);
    easingParam_.posEasing->Reset();
   
    // rotate
    easingParam_.rotateEasing->SetAdaptValue(&easingParam_.rotateEaseValue);
    easingParam_.rotateEasing->Reset();
}

void (JumpAttackUI::* JumpAttackUI::spFuncTable_[])() = {
    &JumpAttackUI::Wait,
    &JumpAttackUI::Open,
    &JumpAttackUI::Close,
};
