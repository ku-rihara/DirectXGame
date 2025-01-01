#pragma once


#include"Player/Weapon/BasePlayerHand.h"


class PlayerHandRight:public BasePlayerHand {
private:
	
private:

	///===========================================
	/// private variant
	///=============================================
	std::string threeComboName_;
	Vector3 combo3MovePos_;
	std::unique_ptr<Object3d>combo3MoveObj_;
	
public:
	PlayerHandRight();
	///===========================================
	/// public method
	///=============================================

	void Init()override;
	void Update()override;
	void Draw(const ViewProjection& viewprojection)override;

	void RailUpdate(const float& speed)override;
	void AdjustParm()override;
	void SaveAndLoad()override;



	void SetParent(WorldTransform* parent);
};