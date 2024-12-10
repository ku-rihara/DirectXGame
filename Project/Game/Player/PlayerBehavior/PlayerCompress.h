#pragma once

#include"BasePlayerBehavior.h"
#include"math/Easing.h"

class PlayerCompress : public BasePlayerBehavior {
private:

	enum class Patern{
		COMPRESS,
		COLLTIME,
		REVERSE,
		CHANGEROOT,
	};
	Patern patern_;
private:

/// ===================================================
///private varians
/// ===================================================

	float collTime_;
	float kCollTime_;

	Easing ease_;
	

public:
	//コンストラクタ
	PlayerCompress(Player* boss);
	~PlayerCompress();

	void Update()override;

	void Debug()override;

};
