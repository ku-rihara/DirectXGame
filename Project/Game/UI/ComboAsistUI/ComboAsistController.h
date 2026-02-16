#pragma once
// AsistParts
#include "AsistParts/ComboAsistArrowUI.h"
#include "AsistParts/ComboAsistButtonUI.h"
// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// Easing
#include "Easing/Easing.h"
// math
#include "Vector2.h"
// std
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class Player;
class PlayerComboAttackController;
class PlayerComboAttackData;

/// <summary>
/// コンボアシストUI管理クラス
/// </summary>
class ComboAsistController {
public:
    // コンボの1ステップ情報
    struct ComboStep {
        int32_t gamepadButton;
        bool isUnlocked;
        std::string attackName;
    };

    // コンボの1パス
    struct ComboPath {
        std::vector<ComboStep> steps;
    };

    // 分岐情報
    struct BranchInfo {
        int32_t divergeIndex; // メインパスから分岐する位置
        ComboPath path;       // 分岐パス
    };

    // 開始ボタンでグループ化されたパス群
    struct ComboPathGroup {
        int32_t startButton = 0;
        ComboPath mainPath;
        std::vector<BranchInfo> branches;

        // UI要素（メインパス用）
        std::vector<std::unique_ptr<ComboAsistButtonUI>> mainButtonUIs;
        std::vector<std::unique_ptr<ComboAsistArrowUI>> mainArrowUIs;

        // UI要素（分岐パス用）
        std::vector<std::vector<std::unique_ptr<ComboAsistButtonUI>>> branchButtonUIs;
        std::vector<std::unique_ptr<ComboAsistArrowUI>> branchArrowUIs; 
        std::vector<std::vector<std::unique_ptr<ComboAsistArrowUI>>> branchInnerArrowUIs;
    };

public:
    ComboAsistController()  = default;
    ~ComboAsistController() = default;

    void Init();
    void Update();
    void AdjustParam();

    //=== スライドイン・アウト ===
    void StartSlideIn();
    void StartSlideOut();
    void UpdateSlide(float deltaTime);
    void ApplySlideOffset();

private:
    //=== パス構築 ===
    void BuildComboPaths();
    void BuildPathsRecursive(
        PlayerComboAttackData* attack,
        std::vector<ComboStep>& currentPath,
        std::unordered_set<std::string>& visited,
        std::vector<ComboPath>& outPaths);
    void ClassifyPathsIntoGroup(ComboPathGroup& group, std::vector<ComboPath>& paths);

    //=== UI生成 ===
    void CreateAllUI();
    void CreateGroupUI(ComboPathGroup& group, int32_t& currentRow);
 

    //=== UI配置 ===
    Vector2 CalcButtonPosition(int32_t col, int32_t row) const;
    Vector2 CalcArrowPosition(const Vector2& fromPos, const Vector2& toPos) const;
    float CalcArrowRotation(const Vector2& fromPos, const Vector2& toPos) const;
    void PlaceArrow(ComboAsistArrowUI& arrow, const Vector2& fromButtonPos, const Vector2& toButtonPos);

    //=== ロック同期 ===
    void SyncUnlockStates();
    void SyncGroupUnlockStates(ComboPathGroup& group);

    //=== 攻撃発動演出 ===
    void UpdateComboState();
    void SetGroupActiveOutLines(ComboPathGroup& group, const std::unordered_set<std::string>& activeAttacks);
    void PlayPushScalingForAttack(ComboPathGroup& group, const std::string& attackName);

    //=== トグル入力 ===
    void CheckToggleInput();

    //=== パラメータ ===
    void RegisterParams();

private:
    Player* pPlayer_                                = nullptr;
    PlayerComboAttackController* pAttackController_ = nullptr;
    KetaEngine::GlobalParameter* globalParameter_   = nullptr;
    std::string groupName_                          = "ComboAsistUI";

    // 攻撃発動演出用
    std::unordered_set<std::string> playedAttacks_;
    std::string prevBehaviorName_;

    // Y開始 / X開始のグループ
    ComboPathGroup yGroup_;
    ComboPathGroup xGroup_;

    // スライドイン・アウト
    KetaEngine::Easing<float> slideInEasing_;
    KetaEngine::Easing<float> slideOutEasing_;
    KetaEngine::Easing<float>* activeSlideEasing_ = nullptr;
    float slideOffsetX_ = 0.0f;
    bool isVisible_     = false;
    bool isSliding_     = false;

    // レイアウトパラメータ
    Vector2 basePosition_;
    Vector2 arrowOffset_;  // ボタン中心からの矢印オフセット
    float columnSpacing_;  // ボタン間の横間隔
    float rowSpacing_;     // パス間の縦間隔
    float branchYOffset_;  // 分岐ボタンのY座標オフセット
    float buttonScale_;    // ボタンUIのスケール
    float arrowScale_;     // 矢印UIのスケール

public:
    void SetAttackController(PlayerComboAttackController* controller) { pAttackController_ = controller; }
    void SetPlayer(Player* player) { pPlayer_ = player; }
};
