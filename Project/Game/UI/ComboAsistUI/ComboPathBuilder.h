#pragma once

#include "Player/ComboCreator/PlayerComboAttackData.h"
// math
#include "Vector2.h"
// std
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

class PlayerComboAttackController;
class PlayerComboAttackData;

/// <summary>
/// コンボパス構築クラス
/// </summary>
class ComboPathBuilder {
public:
	/// コンボの1ステップ
	struct ComboStep {
		int32_t gamepadButton;
		bool isUnlocked;
		bool isAutoAdvance = false; 
		std::string attackName;
	};

	/// コンボの1経路
	struct ComboPath {
		std::vector<ComboStep> steps;
	};

	/// 分岐情報
	struct BranchInfo {
		int32_t divergeIndex; 
		ComboPath path;      
	};

	/// 開始ボタンでグループ化されたパス群
	struct ComboPathGroup {
		int32_t startButton = 0;
		ComboPath mainPath;               
		std::vector<BranchInfo> branches;  
	};

public:
	ComboPathBuilder()  = default;
	~ComboPathBuilder() = default;

	/// コンボツリーからX/Yグループのパスを構築する
	void Build(PlayerComboAttackController* attackController,
	           PlayerComboAttackData::TriggerCondition triggerCondition);

	// グループ参照の取得
	const ComboPathGroup& GetXGroup() const { return xGroup_; }
	const ComboPathGroup& GetYGroup() const { return yGroup_; }

private:
	/// 攻撃データを再帰的に辿り、全パスを列挙する
	void CollectPathsRecursive(
		PlayerComboAttackData* attack,
		std::vector<ComboStep>& currentPath,
		std::unordered_set<std::string>& visited,
		std::vector<ComboPath>& outPaths);

	/// パス群をメインパス（最長）と分岐に分類してグループに格納する
	void ClassifyPaths(ComboPathGroup& group, std::vector<ComboPath>& paths);

private:
	PlayerComboAttackController* pAttackController_ = nullptr;

	// X / Y 開始ボタンごとのグループ
	ComboPathGroup xGroup_;
	ComboPathGroup yGroup_;
};
