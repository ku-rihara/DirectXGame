#include "Audience.h"
// Behavior
#include "Behavior/AudienceRoot.h"
// Combo
#include "Combo/Combo.h"
// Math
#include "MathFunction.h"
// ImGui
#include <imgui.h>

void Audience::Init(int32_t index) {
    BaseObject::Init();

    // グループ名を設定
    audienceIndex_ = index;
    groupName_ += std::to_string(audienceIndex_);

    // グローバルパラメータ
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);
    
    particlePlayer_ = std::make_unique<KetaEngine::ParticlePlayer>();
    particlePlayer_->Init();

    // 列の適応
    seatSide_ = static_cast<SeatSide>(seatSideIndex_);

    // アニメーションオブジェクト作成
    CreateObject();

    // behavior
    ChangeBehavior(std::make_unique<AudienceRoot>(this,false));
}

void Audience::CreateObject() {
    // オブジェ生成
    objAnimation_.reset(KetaEngine::Object3DAnimation::CreateModel("AudienceJump.gltf"));
    // 初期化
    objAnimation_->Init();
    // アニメーション追加
    objAnimation_->Add("AudienceDisAppear.gltf");
    // transform初期化設定
    objAnimation_->transform_.SetIsAdaptDirectScale(true);
    objAnimation_->transform_.Init();
    objAnimation_->transform_.SetParent(&baseTransform_);
    objAnimation_->transform_.scale_ = Vector3::ZeroVector();
}

void Audience::Update() {

    // Behavior更新
    behavior_->Update();
  
    // Particleの更新処理
    particlePlayer_->Update();
    particlePlayer_->SetTargetPosition(objAnimation_->transform_.GetWorldPos());

    // XPositionを設定
    objAnimation_->transform_.translation_.x = positionX_;
    // Y回転設定
    RotateYChangeBySeatSide(seatSide_);

    // 行列更新など
    BaseObject::Update();
}

void Audience::RotateYChangeBySeatSide(SeatSide seatSide) {
    // 座席の位置によってRotateYを変化させる
    switch (seatSide) {
    case SeatSide::LEFT:
        baseTransform_.rotation_.y = ToRadian(-90.0f);
        break;
    case SeatSide::RIGHT:
        baseTransform_.rotation_.y = ToRadian(90.0f);
        break;
    case SeatSide::FRONT:
        baseTransform_.rotation_.y = ToRadian(0.0f);
        break;
    case SeatSide::BACK:
        baseTransform_.rotation_.y = ToRadian(180.0f);
        break;
    }
}

void Audience::AppearByComboLevel(int32_t level) {

    // スポーン該当か確認
    if (appearComboLevel_ != level) {
        return;
    }

    // Rootを取得
    AudienceRoot* audienceRoot = GetAudienceRoot();
    if (!audienceRoot) {
        return;
    }

    // スポーンモードに移行
    particlePlayer_->Play("AppearEffect", "Audience");
    audienceRoot->ChangeAppearMode();
}

void Audience::DisAppearByComboLevel(int32_t level) {

    // クローズ該当か確認
    if (appearComboLevel_ > level) {
        return;
    }

    // Rootを取得
    AudienceRoot* audienceRoot = GetAudienceRoot();
    if (!audienceRoot) {
        return;
    }

    // クローズモードに移行
    audienceRoot->ChangeCloseMode();
}

void Audience::RegisterParams() {
    // パラメータ登録
    globalParameter_->Regist<int32_t>(groupName_, "SeatsRow", &seatsRowIndex_);
    globalParameter_->Regist<int32_t>(groupName_, "SeatSide", &seatSideIndex_);
    globalParameter_->Regist<float>(groupName_, "positionX", &positionX_);
    globalParameter_->Regist<int32_t>(groupName_, "appearComboLevel", &appearComboLevel_);
    globalParameter_->Regist<int32_t>(groupName_, "seatRowNum", &seatRowNum_);
}
void Audience::AdjustParam() {

    ImGui::SeparatorText(groupName_.c_str());
    ImGui::PushID(groupName_.c_str());

    // 座席の列
    ImGui::InputInt("座席の列番号", &seatRowNum_);
    seatRowNum_ = std::clamp(seatRowNum_, 0, 10);

    // SeatSideのコンボボックス
    const char* seatSideItems[] = {"LEFT", "RIGHT", "FRONT", "BACK"};
    if (ImGui::Combo("Seat Side", &seatSideIndex_, seatSideItems, IM_ARRAYSIZE(seatSideItems))) {
        seatSide_ = static_cast<SeatSide>(seatSideIndex_);
    }

    ImGui::DragFloat("X座標のオフセット", &positionX_, 0.1f);
    ImGui::InputInt("出現するコンボレベル", &appearComboLevel_);
    appearComboLevel_ = std::clamp(appearComboLevel_, 0, kComboLevel - 1);

    // セーブ、ロード
    globalParameter_->ParamSaveForImGui(groupName_, folderName_);
    globalParameter_->ParamLoadForImGui(groupName_, folderName_);

    ImGui::PopID();
}

void Audience::AdaptPosition(const Vector2& ZYBasePos) {
    // Position適応
    objAnimation_->transform_.translation_.y = ZYBasePos.y;
    objAnimation_->transform_.translation_.z = ZYBasePos.x;
}

void Audience::ChangeBehavior(std::unique_ptr<BaseAudienceBehavior> behavior) {
    // Behabior切り替え
    behavior_ = std::move(behavior);
}

AudienceRoot* Audience::GetAudienceRoot() const {
    if (AudienceRoot* audienceRoot = dynamic_cast<AudienceRoot*>(behavior_.get())) {
        return audienceRoot;
    }
    return nullptr;
}

void Audience::SetBaseScale(Vector3 scale) {
    baseTransform_.scale_ = scale;
}