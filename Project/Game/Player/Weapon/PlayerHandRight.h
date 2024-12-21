#pragma once


#include"Player/Weapon/BasePlayerHand.h"


class PlayerHandRight:public BasePlayerHand {
private:
	
private:

	///===========================================
	/// private variant
	///=============================================

	
public:
	PlayerHandRight();
	///===========================================
	/// public method
	///=============================================

	void Init()override;
	void Update()override;
	void Draw(const ViewProjection& viewprojection)override;

	void AdjustParm()override;
	void SaveAndLoad()override;
};