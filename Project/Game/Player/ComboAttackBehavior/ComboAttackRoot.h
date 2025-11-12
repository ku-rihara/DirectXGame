#pragma once

#include"BaseComboAattackBehavior.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"


class ComboAttackRoot : public BaseComboAattackBehavior {

private:
	enum class AttackPatern {
		NORMAL,
		JUMP,
	};
private:

	/// ===================================================
	///private varians
	/// ===================================================

	AttackPatern attackPatern_;

public:
	//コンストラクタ
    ComboAttackRoot(Player* player);
	~ComboAttackRoot();

	void Init() override;
	void Update()override;
	void Debug()override;

	void JudgeAttackPattern(); //攻撃パターンの判断　
    bool CheckConditionMuch(const PlayerComboAttackData::TriggerCondition& condition);


};
