#pragma once

#include"BaseComboAattackBehavior.h"
#include"Easing.h"

class ComboAttackFirst : public BaseComboAattackBehavior {
private:
	
	/// ===================================================
	///private varians
	/// ===================================================

	Vector3 targetPos_;   /// ターゲット座標
	Vector3 startPos_;    /// スタート座標


public:
	//コンストラクタ
	ComboAttackFirst(Player* boss);
	~ComboAttackFirst();

	void Update()override;

	void Debug()override;

};
