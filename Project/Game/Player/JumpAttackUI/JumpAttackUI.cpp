#include "JumpAttackUI.h"
#include "Frame/Frame.h"
#include <imgui.h>

void JumpAttackUI::Init() {

    // スプライト生成
    bottoms_[X].sprite.reset(Sprite::Create("OperateUI/OperateX.png"));
    bottoms_[Y].sprite.reset(Sprite::Create("OperateUI/OperateY.png"));
    bottoms_[B].sprite.reset(Sprite::Create("OperateUI/OperateB.png"));
    bottoms_[A].sprite.reset(Sprite::Create("OperateUI/OperateA.png"));

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
        Vector2 offset = bottoms_[i].posValueOffset + positionScreenV2;

        bottoms_[i].sprite->transform_.pos      = bottoms_[i].discreteDirection * easingParam_.posEaseValue + offset;
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
        bottoms_[Y].discreteDirection = Vector2(0.0f, -1.0f);
        break;
    case B:
        bottoms_[B].discreteDirection = Vector2(1.0f, 0.0f);
        break;
    case A:
        bottoms_[A].discreteDirection = Vector2(0.0f, 1.0f);
        break;
    default:
        break;
    }
}

std::string JumpAttackUI::BottomNameByType(const size_t& TypeNum) {
    switch (TypeNum) {
    case X:
        return "X";
        break;
    case Y:
        return "Y";
        break;
    case B:
        return "B";
        break;
    case A:
        return "A";
        break;
    default:
        break;
    }
    return "";
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
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        globalParameter_->Bind(groupName_, "posValueOffset" + BottomNameByType(i), &bottoms_[i].posValueOffset);
    }
}

void JumpAttackUI::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
            ImGui::DragFloat2(("PosValueOffset" + BottomNameByType(i)).c_str(), &bottoms_[i].posValueOffset.x, 0.1f);
        }

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
    easingParam_.scaleEasing->Init("PlayerOperateUIScaling", "PlayerOperateUIScaling.json");
    easingParam_.posEasing->Init("PlayerOperateUIOffsetPos", "PlayerOperateUIOffsetPos.json");
    easingParam_.rotateEasing->Init("PlayerOperateUIRotate", "PlayerOperateUIRotate.json");

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

void JumpAttackUI::StartOpen() {

    state_ = State::OPEN;
}
void JumpAttackUI::StartClose() {

    state_ = State::CLOSE;
}

void (JumpAttackUI::* JumpAttackUI::spFuncTable_[])() = {
    &JumpAttackUI::Wait,
    &JumpAttackUI::Open,
    &JumpAttackUI::Close,
};
