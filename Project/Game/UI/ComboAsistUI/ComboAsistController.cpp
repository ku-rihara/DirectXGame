#include "ComboAsistController.h"
#include "Frame/Frame.h"
#include "Input/Input.h"
#include "input/InputData.h"
#include "Player/Player.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include <algorithm>
#include <cmath>
#include <imgui.h>

void ComboAsistController::Init() {
    if (!pAttackController_) {
        return;
    }

    // GlobalParameter初期化
    globalParameter_ = KetaEngine::GlobalParameter::GetInstance();
    globalParameter_->CreateGroup(groupName_);
    RegisterParams();
    globalParameter_->SyncParamForGroup(groupName_);

    // ボタン値設定
    xGroup_.startButton = ToXInputButtonFlag(GamepadButton::X);
    yGroup_.startButton = ToXInputButtonFlag(GamepadButton::Y);

    BuildComboPaths();
    CreateAllUI();

    // スライドイージング初期化
    slideInEasing_.Init("ComboAsistSlideIn.json");
    slideInEasing_.SetAdaptValue(&slideOffsetX_);
    slideInEasing_.Reset();

    slideOutEasing_.Init("ComboAsistSlideOut.json");
    slideOutEasing_.SetAdaptValue(&slideOffsetX_);
    slideOutEasing_.Reset();

    activeSlideEasing_ = nullptr;
    isVisible_ = false;
    isSliding_ = false;

    // 初期状態は画面外に隠す
    slideOffsetX_ = slideInEasing_.GetStartValue();
    ApplySlideOffset();
}

void ComboAsistController::Update() {
    SyncUnlockStates();
    UpdateComboState();
    CheckToggleInput();
    UpdateSlide(KetaEngine::Frame::DeltaTime());
    ApplySlideOffset();
}

///==========================================================
/// コンボツリーからパスを構築
///==========================================================
void ComboAsistController::BuildComboPaths() {
    const auto& attacks = pAttackController_->GetAllAttacks();

    int32_t yButton = ToXInputButtonFlag(GamepadButton::Y);
    int32_t xButton = ToXInputButtonFlag(GamepadButton::X);

    for (const auto& attack : attacks) {
        if (!pAttackController_->IsFirstAttack(attack->GetGroupName())) {
            continue;
        }

        // 地上条件のみ
        auto condition = attack->GetAttackParam().triggerParam.condition;
        if (condition != PlayerComboAttackData::TriggerCondition::GROUND && condition != PlayerComboAttackData::TriggerCondition::BOTH) {
            continue;
        }

        int32_t startButton = attack->GetAttackParam().triggerParam.gamePadBottom;

        // 全パス列挙
        std::vector<ComboPath> paths;
        std::vector<ComboStep> currentPath;
        std::unordered_set<std::string> visited;

        ComboStep firstStep{startButton, attack->GetAttackParam().isUnlocked, attack->GetGroupName()};
        currentPath.push_back(firstStep);
        visited.insert(attack->GetGroupName());

        BuildPathsRecursive(attack.get(), currentPath, visited, paths);

        if (paths.empty() && !currentPath.empty()) {
            paths.push_back({currentPath});
        }

        // グループに振り分け
        if (startButton == xButton) {
            ClassifyPathsIntoGroup(xGroup_, paths);
        } else if (startButton == yButton) {
            ClassifyPathsIntoGroup(yGroup_, paths);
        }
    }
}

///==========================================================
/// パスをメインパスと分岐に分類
///==========================================================
void ComboAsistController::ClassifyPathsIntoGroup(ComboPathGroup& group, std::vector<ComboPath>& paths) {
    if (paths.empty()) {
        return;
    }

    // 最長パスをメインパスにする
    std::sort(paths.begin(), paths.end(), [](const ComboPath& a, const ComboPath& b) {
        return a.steps.size() > b.steps.size();
    });

    group.mainPath = std::move(paths[0]);

    // 残りを分岐として登録
    for (size_t i = 1; i < paths.size(); ++i) {
        BranchInfo branch;

        // メインパスとの分岐点を求める
        branch.divergeIndex = 0;
        size_t minLen       = (std::min)(group.mainPath.steps.size(), paths[i].steps.size());
        for (size_t s = 0; s < minLen; ++s) {
            if (group.mainPath.steps[s].attackName != paths[i].steps[s].attackName) {
                break;
            }
            branch.divergeIndex = static_cast<int32_t>(s) + 1;
        }

        branch.path = std::move(paths[i]);
        group.branches.push_back(std::move(branch));
    }
}

///==========================================================
/// 再帰パス列挙
///==========================================================
void ComboAsistController::BuildPathsRecursive(
    PlayerComboAttackData* attack,
    std::vector<ComboStep>& currentPath,
    std::unordered_set<std::string>& visited,
    std::vector<ComboPath>& outPaths) {

    const auto& branches = attack->GetComboBranches();

    // 入力のある分岐があるか
    bool hasInputBranch = false;
    for (const auto& branch : branches) {
        if (branch->GetGamepadButton() != 0 && !branch->GetNextAttackName().empty()) {
            hasInputBranch = true;
            break;
        }
    }

    if (branches.empty() || !hasInputBranch) {
        outPaths.push_back({currentPath});
        return;
    }

    for (const auto& branch : branches) {
        const std::string& nextName = branch->GetNextAttackName();
        int32_t branchButton        = branch->GetGamepadButton();

        if (branchButton == 0 || nextName.empty()) {
            continue;
        }
        if (visited.find(nextName) != visited.end()) {
            continue;
        }

        PlayerComboAttackData* nextAttack = pAttackController_->GetAttackByName(nextName);
        if (!nextAttack) {
            continue;
        }

        ComboStep step{branchButton, nextAttack->GetAttackParam().isUnlocked, nextName};
        currentPath.push_back(step);
        visited.insert(nextName);

        BuildPathsRecursive(nextAttack, currentPath, visited, outPaths);

        currentPath.pop_back();
        visited.erase(nextName);
    }
}

///==========================================================
/// 全UI生成
///==========================================================
void ComboAsistController::CreateAllUI() {
    int32_t currentRow = 0;
    CreateGroupUI(yGroup_, currentRow);
    CreateGroupUI(xGroup_, currentRow);
}

///==========================================================
/// グループのUI生成
///==========================================================
void ComboAsistController::CreateGroupUI(ComboPathGroup& group, int32_t& currentRow) {
    if (group.mainPath.steps.empty()) {
        return;
    }

    // --- メインパスのボタン ---
    int32_t mainRow = currentRow;
    for (size_t i = 0; i < group.mainPath.steps.size(); ++i) {
        const auto& step = group.mainPath.steps[i];

        auto btn = std::make_unique<ComboAsistButtonUI>();
        btn->Init(step.gamepadButton, step.isUnlocked);
        btn->SetPosition(CalcButtonPosition(static_cast<int32_t>(i), mainRow));
        btn->SetScale(buttonScale_);
        group.mainButtonUIs.push_back(std::move(btn));
    }

    // --- メインパスの矢印 ---
    for (size_t i = 0; i + 1 < group.mainPath.steps.size(); ++i) {
        Vector2 fromPos = CalcButtonPosition(static_cast<int32_t>(i), mainRow);
        Vector2 toPos   = CalcButtonPosition(static_cast<int32_t>(i + 1), mainRow);

        auto arrow = std::make_unique<ComboAsistArrowUI>();
        arrow->Init();
        PlaceArrow(*arrow, fromPos, toPos);
        arrow->SetScale(arrowScale_);
        group.mainArrowUIs.push_back(std::move(arrow));
    }

    currentRow++;

    // --- 分岐パスのUI ---
    group.branchButtonUIs.resize(group.branches.size());
    group.branchInnerArrowUIs.resize(group.branches.size());

    for (size_t bIdx = 0; bIdx < group.branches.size(); ++bIdx) {
        const auto& branchInfo = group.branches[bIdx];
        int32_t branchRow      = currentRow;
        auto& buttons          = group.branchButtonUIs[bIdx];
        auto& innerArrows      = group.branchInnerArrowUIs[bIdx];

        // 分岐元から分岐先への斜め矢印
        if (branchInfo.divergeIndex > 0 && branchInfo.divergeIndex < static_cast<int32_t>(branchInfo.path.steps.size())) {

            Vector2 fromPos = CalcButtonPosition(branchInfo.divergeIndex - 1, mainRow);
            Vector2 toPos   = CalcButtonPosition(branchInfo.divergeIndex, branchRow);

            auto branchArrow = std::make_unique<ComboAsistArrowUI>();
            branchArrow->Init();
            PlaceArrow(*branchArrow, fromPos, toPos);
            branchArrow->SetScale(arrowScale_);
            group.branchArrowUIs.push_back(std::move(branchArrow));
        }

        // 分岐パスのボタン
        for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
            const auto& step = branchInfo.path.steps[i];

            auto btn = std::make_unique<ComboAsistButtonUI>();
            btn->Init(step.gamepadButton, step.isUnlocked);
            btn->SetPosition(CalcButtonPosition(static_cast<int32_t>(i), branchRow));
            btn->SetScale(buttonScale_);
            buttons.push_back(std::move(btn));
        }

        // 分岐パス内の矢印
        for (size_t i = branchInfo.divergeIndex; i + 1 < branchInfo.path.steps.size(); ++i) {
            Vector2 fromPos = CalcButtonPosition(static_cast<int32_t>(i), branchRow);
            Vector2 toPos   = CalcButtonPosition(static_cast<int32_t>(i + 1), branchRow);

            auto arrow = std::make_unique<ComboAsistArrowUI>();
            arrow->Init();
            PlaceArrow(*arrow, fromPos, toPos);
            arrow->SetScale(arrowScale_);
            innerArrows.push_back(std::move(arrow));
        }

        currentRow++;
    }
}

///==========================================================
/// ボタン位置の計算
///==========================================================
Vector2 ComboAsistController::CalcButtonPosition(int32_t col, int32_t row) const {
    return Vector2(
        basePosition_.x + col * columnSpacing_,
        basePosition_.y + row * (rowSpacing_ + branchYOffset_));
}

///==========================================================
/// 矢印の位置計算（2つのボタン中心間の中点）
///==========================================================
Vector2 ComboAsistController::CalcArrowPosition(const Vector2& fromPos, const Vector2& toPos) const {
    return Vector2(
        fromPos.x + arrowOffset_.x,
        fromPos.y + arrowOffset_.y + (toPos.y - fromPos.y) * 0.5f);
}

///==========================================================
/// 矢印の回転角度計算（ラジアン）
///==========================================================
float ComboAsistController::CalcArrowRotation(const Vector2& fromPos, const Vector2& toPos) const {
    float dx = toPos.x - fromPos.x;
    float dy = toPos.y - fromPos.y;
    return std::atan2(dy, dx);
}

///==========================================================
/// 矢印の配置（位置 + 回転）
///==========================================================
void ComboAsistController::PlaceArrow(ComboAsistArrowUI& arrow, const Vector2& fromButtonPos, const Vector2& toButtonPos) {
    Vector2 arrowPos = CalcArrowPosition(fromButtonPos, toButtonPos);
    float rotation   = CalcArrowRotation(fromButtonPos, toButtonPos);

    arrow.SetPosition(arrowPos);
    arrow.SetRotation(rotation);
}

///==========================================================
/// スライドイン開始
///==========================================================
void ComboAsistController::StartSlideIn() {
    activeSlideEasing_ = &slideInEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
    isVisible_ = true;
}

///==========================================================
/// スライドアウト開始
///==========================================================
void ComboAsistController::StartSlideOut() {
    activeSlideEasing_ = &slideOutEasing_;
    activeSlideEasing_->Reset();
    isSliding_ = true;
}

///==========================================================
/// スライド更新
///==========================================================
void ComboAsistController::UpdateSlide(float deltaTime) {
    if (!isSliding_ || !activeSlideEasing_) {
        return;
    }
    activeSlideEasing_->Update(deltaTime);
    if (activeSlideEasing_->IsFinished()) {
        isSliding_ = false;
        // スライドアウト完了時に非表示
        if (activeSlideEasing_ == &slideOutEasing_) {
            isVisible_ = false;
        }
    }
}

///==========================================================
/// スライドオフセット適用
///==========================================================
void ComboAsistController::ApplySlideOffset() {
    auto applyGroup = [&](ComboPathGroup& group, int32_t mainRow) {
        // メインパスのボタン
        for (size_t i = 0; i < group.mainButtonUIs.size(); ++i) {
            Vector2 pos = CalcButtonPosition(static_cast<int32_t>(i), mainRow);
            pos.x += slideOffsetX_;
            group.mainButtonUIs[i]->SetPosition(pos);
        }
        // メインパスの矢印
        for (size_t i = 0; i + 1 < group.mainButtonUIs.size(); ++i) {
            Vector2 fromPos = CalcButtonPosition(static_cast<int32_t>(i), mainRow);
            Vector2 toPos   = CalcButtonPosition(static_cast<int32_t>(i + 1), mainRow);
            fromPos.x += slideOffsetX_;
            toPos.x += slideOffsetX_;
            if (i < group.mainArrowUIs.size()) {
                PlaceArrow(*group.mainArrowUIs[i], fromPos, toPos);
            }
        }
        // 分岐パス
        int32_t branchRowBase = mainRow + 1;
        size_t branchArrowIdx = 0;
        for (size_t bIdx = 0; bIdx < group.branches.size(); ++bIdx) {
            const auto& branchInfo = group.branches[bIdx];
            int32_t branchRow      = branchRowBase + static_cast<int32_t>(bIdx);
            auto& buttons          = group.branchButtonUIs[bIdx];
            auto& innerArrows      = group.branchInnerArrowUIs[bIdx];

            // 分岐元→分岐先の斜め矢印
            if (branchInfo.divergeIndex > 0 && branchInfo.divergeIndex < static_cast<int32_t>(branchInfo.path.steps.size())) {
                Vector2 fromPos = CalcButtonPosition(branchInfo.divergeIndex - 1, mainRow);
                Vector2 toPos   = CalcButtonPosition(branchInfo.divergeIndex, branchRow);
                fromPos.x += slideOffsetX_;
                toPos.x += slideOffsetX_;
                if (branchArrowIdx < group.branchArrowUIs.size()) {
                    PlaceArrow(*group.branchArrowUIs[branchArrowIdx], fromPos, toPos);
                }
                branchArrowIdx++;
            }

            // 分岐ボタン
            size_t btnIdx = 0;
            for (size_t i = branchInfo.divergeIndex; i < branchInfo.path.steps.size(); ++i) {
                if (btnIdx < buttons.size()) {
                    Vector2 pos = CalcButtonPosition(static_cast<int32_t>(i), branchRow);
                    pos.x += slideOffsetX_;
                    buttons[btnIdx]->SetPosition(pos);
                }
                btnIdx++;
            }

            // 分岐内矢印
            size_t arrowIdx = 0;
            for (size_t i = branchInfo.divergeIndex; i + 1 < branchInfo.path.steps.size(); ++i) {
                Vector2 fromPos = CalcButtonPosition(static_cast<int32_t>(i), branchRow);
                Vector2 toPos   = CalcButtonPosition(static_cast<int32_t>(i + 1), branchRow);
                fromPos.x += slideOffsetX_;
                toPos.x += slideOffsetX_;
                if (arrowIdx < innerArrows.size()) {
                    PlaceArrow(*innerArrows[arrowIdx], fromPos, toPos);
                }
                arrowIdx++;
            }
        }
    };

    // Y グループ (row 0)
    int32_t yMainRow = 0;
    applyGroup(yGroup_, yMainRow);

    // X グループ (row = yグループが使用した行数の後)
    int32_t xMainRow = 1 + static_cast<int32_t>(yGroup_.branches.size());
    applyGroup(xGroup_, xMainRow);
}

///==========================================================
/// トグル入力チェック
///==========================================================
void ComboAsistController::CheckToggleInput() {
    if (KetaEngine::Input::IsTriggerPad(0, GamepadButton::DPadRight)) {
        if (isVisible_) {
            StartSlideOut();
        } else {
            StartSlideIn();
        }
    }
}

///==========================================================
/// ロック状態の同期
///==========================================================
void ComboAsistController::SyncUnlockStates() {
    if (!pAttackController_) {
        return;
    }

    SyncGroupUnlockStates(xGroup_);
    SyncGroupUnlockStates(yGroup_);
}

void ComboAsistController::SyncGroupUnlockStates(ComboPathGroup& group) {
    // メインパス
    for (size_t i = 0; i < group.mainPath.steps.size(); ++i) {
        auto* attack = pAttackController_->GetAttackByName(group.mainPath.steps[i].attackName);
        if (attack && i < group.mainButtonUIs.size()) {
            bool unlocked                      = attack->GetAttackParam().isUnlocked;
            group.mainPath.steps[i].isUnlocked = unlocked;
            group.mainButtonUIs[i]->SetUnlocked(unlocked);
        }
    }

    // 分岐パス
    for (size_t bIdx = 0; bIdx < group.branches.size(); ++bIdx) {
        auto& branch  = group.branches[bIdx];
        auto& buttons = group.branchButtonUIs[bIdx];

        size_t btnIdx = 0;
        for (size_t i = branch.divergeIndex; i < branch.path.steps.size(); ++i) {
            auto* attack = pAttackController_->GetAttackByName(branch.path.steps[i].attackName);
            if (attack && btnIdx < buttons.size()) {
                bool unlocked                   = attack->GetAttackParam().isUnlocked;
                branch.path.steps[i].isUnlocked = unlocked;
                buttons[btnIdx]->SetUnlocked(unlocked);
            }
            btnIdx++;
        }
    }
}

///==========================================================
/// 攻撃発動演出の更新
///==========================================================
void ComboAsistController::UpdateComboState() {
    if (!pPlayer_) {
        return;
    }

    auto* comboBehavior = pPlayer_->GetComboBehavior();
    if (!comboBehavior) {
        return;
    }

    const std::string& behaviorName = comboBehavior->GetName();

    if (behaviorName == "ComboAttackRoot") {
        // ComboAttackRootに戻ったら全リセット
        if (!playedAttacks_.empty()) {
            playedAttacks_.clear();
            SetGroupActiveOutLines(yGroup_, playedAttacks_);
            SetGroupActiveOutLines(xGroup_, playedAttacks_);
        }
    } else {
        // 新たな攻撃が発動した瞬間にPushScalingアニメ再生
        bool isNewAttack = (behaviorName != prevBehaviorName_);
        playedAttacks_.insert(behaviorName);
        SetGroupActiveOutLines(yGroup_, playedAttacks_);
        SetGroupActiveOutLines(xGroup_, playedAttacks_);

        if (isNewAttack) {
            PlayPushScalingForAttack(yGroup_, behaviorName);
            PlayPushScalingForAttack(xGroup_, behaviorName);
        }
    }

    prevBehaviorName_ = behaviorName;
}

///==========================================================
/// グループ内ボタンのアクティブアウトライン設定
///==========================================================
void ComboAsistController::SetGroupActiveOutLines(ComboPathGroup& group, const std::unordered_set<std::string>& activeAttacks) {
    // メインパス
    for (size_t i = 0; i < group.mainPath.steps.size(); ++i) {
        if (i < group.mainButtonUIs.size()) {
            bool active = activeAttacks.count(group.mainPath.steps[i].attackName) > 0;
            group.mainButtonUIs[i]->SetActiveOutLine(active);
        }
    }

    // 分岐パス
    for (size_t bIdx = 0; bIdx < group.branches.size(); ++bIdx) {
        const auto& branch = group.branches[bIdx];
        auto& buttons      = group.branchButtonUIs[bIdx];

        size_t btnIdx = 0;
        for (size_t i = branch.divergeIndex; i < branch.path.steps.size(); ++i) {
            if (btnIdx < buttons.size()) {
                bool active = activeAttacks.count(branch.path.steps[i].attackName) > 0;
                buttons[btnIdx]->SetActiveOutLine(active);
            }
            btnIdx++;
        }
    }
}

///==========================================================
/// 攻撃名に対応するボタンのPushScalingアニメーション再生
///==========================================================
void ComboAsistController::PlayPushScalingForAttack(ComboPathGroup& group, const std::string& attackName) {
    // メインパス
    for (size_t i = 0; i < group.mainPath.steps.size(); ++i) {
        if (group.mainPath.steps[i].attackName == attackName && i < group.mainButtonUIs.size()) {
            group.mainButtonUIs[i]->PlayPushScaling();
            return;
        }
    }

    // 分岐パス
    for (size_t bIdx = 0; bIdx < group.branches.size(); ++bIdx) {
        const auto& branch = group.branches[bIdx];
        auto& buttons      = group.branchButtonUIs[bIdx];

        size_t btnIdx = 0;
        for (size_t i = branch.divergeIndex; i < branch.path.steps.size(); ++i) {
            if (branch.path.steps[i].attackName == attackName && btnIdx < buttons.size()) {
                buttons[btnIdx]->PlayPushScaling();
                return;
            }
            btnIdx++;
        }
    }
}

///==========================================================
/// パラメータ登録
///==========================================================
void ComboAsistController::RegisterParams() {
    globalParameter_->Regist(groupName_, "basePosition", &basePosition_);
    globalParameter_->Regist(groupName_, "arrowOffset", &arrowOffset_);
    globalParameter_->Regist(groupName_, "columnSpacing", &columnSpacing_);
    globalParameter_->Regist(groupName_, "rowSpacing", &rowSpacing_);
    globalParameter_->Regist(groupName_, "branchYOffset", &branchYOffset_);
    globalParameter_->Regist(groupName_, "buttonScale", &buttonScale_);
    globalParameter_->Regist(groupName_, "arrowScale", &arrowScale_);
}

///==========================================================
/// パラメータ調整
///==========================================================
void ComboAsistController::AdjustParam() {
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(groupName_.c_str())) {
        ImGui::PushID(groupName_.c_str());

        ImGui::DragFloat2("Base Position", &basePosition_.x, 0.1f);
        ImGui::DragFloat2("Arrow Offset", &arrowOffset_.x, 0.1f);
        ImGui::DragFloat("Column Spacing", &columnSpacing_, 0.1f);
        ImGui::DragFloat("Row Spacing", &rowSpacing_, 0.1f);
        ImGui::DragFloat("Branch Y Offset", &branchYOffset_, 0.1f);
        ImGui::DragFloat("Button Scale", &buttonScale_, 0.01f);
        ImGui::DragFloat("Arrow Scale", &arrowScale_, 0.01f);

        // リアルタイム再配置
        if (ImGui::Button("Rebuild UI")) {
            // UI要素クリア
            yGroup_.mainButtonUIs.clear();
            yGroup_.mainArrowUIs.clear();
            yGroup_.branchButtonUIs.clear();
            yGroup_.branchArrowUIs.clear();
            yGroup_.branchInnerArrowUIs.clear();
            xGroup_.mainButtonUIs.clear();
            xGroup_.mainArrowUIs.clear();
            xGroup_.branchButtonUIs.clear();
            xGroup_.branchArrowUIs.clear();
            xGroup_.branchInnerArrowUIs.clear();
            CreateAllUI();
        }

        // セーブ・ロード
        globalParameter_->ParamSaveForImGui(groupName_);
        globalParameter_->ParamLoadForImGui(groupName_);

        ImGui::PopID();
    }
#endif
}
