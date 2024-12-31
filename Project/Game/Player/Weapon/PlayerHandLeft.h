#pragma once


#include"Player/Weapon/BasePlayerHand.h"



class PlayerHandLeft:public BasePlayerHand {
private:
	
private:

	///===========================================
	/// private variant
	///=============================================
	
	
public:
	PlayerHandLeft();
	///===========================================
	/// public method
	///=============================================

	void Init()override;
	void Update()override;
	void Draw(const ViewProjection& viewprojection)override;

	void RailUpdate()override;
	 void AdjustParm()override;
	 void SaveAndLoad()override;


	  void SetParent(WorldTransform* parent)override;
};