#include "ComboPathBuilder.h"
#include "Player/ComboCreator/PlayerComboAttackController.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"
#include "input/InputData.h"
#include <algorithm>

///==========================================================
/// コンボツリーからパスを構築
///==========================================================
void ComboPathBuilder::Build(
	PlayerComboAttackController* attackController,
	PlayerComboAttackData::TriggerCondition triggerCondition) {

	pAttackController_ = attackController;
	if (!pAttackController_) {
		return;
	}

	const auto& allAttacks = pAttackController_->GetAllAttacks();

	int32_t xButton = ToXInputButtonFlag(GamepadButton::X);
	int32_t yButton = ToXInputButtonFlag(GamepadButton::Y);

	for (const auto& attack : allAttacks) {
		// 最初の攻撃（コンボの起点）だけを処理する
		if (!pAttackController_->IsFirstAttack(attack->GetGroupName())) {
			continue;
		}

		// 指定されたコンディションに一致しなければスキップ
		auto condition = attack->GetAttackParam().triggerParam.condition;
		if (condition != triggerCondition) {
			continue;
		}

		// この攻撃の開始ボタンを取得
		int32_t startButton = attack->GetAttackParam().triggerParam.gamePadBottom;

		// 最初のステップを作成
		ComboStep firstStep;
		firstStep.gamepadButton = startButton;
		firstStep.isUnlocked    = attack->GetAttackParam().isUnlocked;
		firstStep.isAutoAdvance = attack->GetAttackParam().timingParam.isAutoAdvance;
		firstStep.attackName    = attack->GetGroupName();

		// 再帰的に全パスを収集
		std::vector<ComboPath> paths;
		std::vector<ComboStep> currentPath;
		std::unordered_set<std::string> visited;

		currentPath.push_back(firstStep);
		visited.insert(attack->GetGroupName());

		CollectPathsRecursive(attack.get(), currentPath, visited, paths);

		// 分岐が無い場合でも、現在のパス（1ステップだけ）を結果に含める
		if (paths.empty() && !currentPath.empty()) {
			paths.push_back({currentPath});
		}

		// 開始ボタンに応じてグループに分類
		if (startButton == xButton) {
			ClassifyPaths(xGroup_, paths);
		} else if (startButton == yButton) {
			ClassifyPaths(yGroup_, paths);
		}
	}
}

///==========================================================
/// 再帰パス列挙
///==========================================================
void ComboPathBuilder::CollectPathsRecursive(
	PlayerComboAttackData* attack,
	std::vector<ComboStep>& currentPath,
	std::unordered_set<std::string>& visited,
	std::vector<ComboPath>& outPaths) {

	const auto& branches = attack->GetComboBranches();

	// ボタン入力を伴う有効な分岐があるかチェック
	bool hasValidBranch = false;
	for (const auto& branch : branches) {
		if (branch->GetGamepadButton() != 0 && !branch->GetNextAttackName().empty()) {
			hasValidBranch = true;
			break;
		}
	}

	// 有効な分岐が無ければ、ここがコンボの末端 → パスを記録して帰る
	if (branches.empty() || !hasValidBranch) {
		outPaths.push_back({currentPath});
		return;
	}

	// 各分岐を辿る
	for (const auto& branch : branches) {
		const std::string& nextName = branch->GetNextAttackName();
		int32_t branchButton        = branch->GetGamepadButton();

		// ボタン入力の無い分岐や、名前が空の分岐はスキップ
		if (branchButton == 0 || nextName.empty()) {
			continue;
		}

		// 循環防止：既に訪問済みならスキップ
		if (visited.count(nextName) > 0) {
			continue;
		}

		// 次の攻撃データを取得
		PlayerComboAttackData* nextAttack = pAttackController_->GetAttackByName(nextName);
		if (!nextAttack) {
			continue;
		}

		// ステップを追加して再帰
		ComboStep step;
		step.gamepadButton = branchButton;
		step.isUnlocked    = nextAttack->GetAttackParam().isUnlocked;
		step.isAutoAdvance = nextAttack->GetAttackParam().timingParam.isAutoAdvance;
		step.attackName    = nextName;

		currentPath.push_back(step);
		visited.insert(nextName);

		CollectPathsRecursive(nextAttack, currentPath, visited, outPaths);

		// バックトラック
		currentPath.pop_back();
		visited.erase(nextName);
	}
}

///==========================================================
/// パス群をメインパスと分岐に分類
///==========================================================
void ComboPathBuilder::ClassifyPaths(ComboPathGroup& group, std::vector<ComboPath>& paths) {
	if (paths.empty()) {
		return;
	}

	// 最長パスが先頭に来るようソート
	std::sort(paths.begin(), paths.end(), [](const ComboPath& lhs, const ComboPath& rhs) {
		return lhs.steps.size() > rhs.steps.size();
	});

	// 先頭（最長）をメインパスとして採用
	group.mainPath = std::move(paths[0]);

	// 残りを分岐として登録
	for (size_t i = 1; i < paths.size(); ++i) {
		BranchInfo branch;

		// メインパスと何ステップ目まで共通かを調べる 
		branch.divergeIndex   = 0;
		const auto& branchSteps = paths[i].steps;
		size_t compareLength    = (std::min)(group.mainPath.steps.size(), branchSteps.size());

		for (size_t s = 0; s < compareLength; ++s) {
			if (group.mainPath.steps[s].attackName != branchSteps[s].attackName) {
				break;
			}
			branch.divergeIndex = static_cast<int32_t>(s) + 1;
		}

		branch.path = std::move(paths[i]);
		group.branches.push_back(std::move(branch));
	}
}
