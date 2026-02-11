#pragma once

#include"BaseComboAttackBehavior.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"


class ComboAttackRoot : public BaseComboAttackBehavior {

private:
	enum class AttackPatern {
		NORMAL,
		JUMP,
	};
private:

	/// ===================================================
	///private variable
	/// ===================================================

	AttackPatern attackPatern_;

public:
	//コンストラクタ
    ComboAttackRoot(Player* player);
	~ComboAttackRoot();

	void Init() override;
    void Update(float atkSpeed) override;
	void Debug()override;

	void JudgeAttackPattern(); //攻撃パターンの判断　
    bool CheckConditionMuch(const PlayerComboAttackData::TriggerCondition& condition);


};
