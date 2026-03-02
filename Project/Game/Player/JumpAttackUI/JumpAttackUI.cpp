#include "JumpAttackUI.h"
#include "Editor/SpriteEaseAnimation/SpriteEaseAnimationPlayer.h"
#include "Frame/Frame.h"
#include <imgui.h>

void JumpAttackUI::Init() {

    // スプライト生成
    bottoms_[X].sprite.reset(KetaEngine::Sprite::Create("OperateUI/OperateX.dds"));
    bottoms_[Y].sprite.reset(KetaEngine::Sprite::Create("OperateUI/OperateY.dds"));
    bottoms_[B].sprite.reset(KetaEngine::Sprite::Create("OperateUI/OperateB.dds"));
    bottoms_[A].sprite.reset(KetaEngine::Sprite::Create("OperateUI/OperateA.dds"));

    // 移動向き決定
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        DirectionByBottom(i);
    }

    // Global parameter
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    EasingInit();

    bottoms_[Y].isNotOperate = true;
    bottoms_[B].isNotOperate = true;
    bottoms_[A].isNotOperate = true;
}

void JumpAttackUI::Update(const Vector3& basePos, const KetaEngine::ViewProjection& viewProjection) {
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

        bottoms_[i].sprite->transform_.pos      = bottoms_[i].discreteDirection * posEasingParam_.posEaseValue + offset;

        // alpha
        if (bottoms_[i].isNotOperate) {
            bottoms_[i].sprite->SetAlpha(notOperateAlpha_);
        } else {

            bottoms_[i].sprite->SetAlpha(1.0f);
        }
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
    posEasingParam_.posEasing->Update(KetaEngine::Frame::DeltaTime());
}
void JumpAttackUI::Close() {
    posEasingParam_.posEasing->Update(KetaEngine::Frame::DeltaTime());
}

void JumpAttackUI::RegisterParams() {
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        globalParameter_->Regist(groupName_, "posValueOffset" + BottomNameByType(i), &bottoms_[i].posValueOffset);
    }
    globalParameter_->Regist(groupName_, "notOperateAlpha", &notOperateAlpha_);
}

void JumpAttackUI::AdjustParam() {

#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
            ImGui::DragFloat2(("PosValueOffset" + BottomNameByType(i)).c_str(), &bottoms_[i].posValueOffset.x, 0.1f);
        }
        ImGui::DragFloat("NotOperateAlpha", &notOperateAlpha_, 0.01f, 0.0f, 1.0f);

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif // _DEBUG
}

void JumpAttackUI::EasingInit() {
    posEasingParam_.posEasing = std::make_unique<KetaEngine::Easing<float>>();

    // pos (方向ベクトル係数として使用するためEasing<float>のまま)
    posEasingParam_.posEasing->Init("PlayerOperateUIOffsetPos.json");
    posEasingParam_.posEasing->SetAdaptValue(&posEasingParam_.posEaseValue);
    posEasingParam_.posEasing->Reset();
}

void JumpAttackUI::StartOpen() {
    state_ = State::OPEN;

    // Scale/Rotation は SpriteEaseAnimation で再生
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        bottoms_[i].sprite->PlaySpriteEaseAnimation("PlayerOperateUIOpen", "JumpAttackUI");
    }

    // pos: 通常状態に設定 (Start → End)
    posEasingParam_.posEasing->SetIsStartEndReverse(false);
    posEasingParam_.posEasing->Reset();
}

void JumpAttackUI::StartClose() {
    state_ = State::CLOSE;

    // Scale/Rotation は SpriteEaseAnimation で再生
    for (size_t i = 0; i < static_cast<size_t>(Type::COUNT); ++i) {
        bottoms_[i].sprite->PlaySpriteEaseAnimation("PlayerOperateUIClose", "JumpAttackUI");
    }

    // pos: 反転状態に設定 (End → Start)
    posEasingParam_.posEasing->SetIsStartEndReverse(true);
    posEasingParam_.posEasing->Reset();
}

void (JumpAttackUI::* JumpAttackUI::spFuncTable_[])() = {
    &JumpAttackUI::Wait,
    &JumpAttackUI::Open,
    &JumpAttackUI::Close,
};
