#pragma once

#include"BaseComboAttackBehavior.h"
#include "Player/ComboCreator/PlayerComboAttackData.h"


/// <summary>
/// コンボ攻撃の起点となる振る舞い(攻撃パターン判定と派生条件の判定を行う)
/// </summary>
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
    Type GetComboType() const override { return Type::Root; }

	void JudgeAttackPattern(); //攻撃パターンの判断　
    bool CheckConditionMuch(const PlayerComboAttackData::TriggerCondition& condition);


};
