#pragma once


#include"Player/Weapon/BasePlayerHand.h"


class PlayerHandRight:public BasePlayerHand {
private:
	
private:

	///===========================================
	/// private variant
	///=============================================
	std::string threeComboName_;
	std::string forthComboName_;

	//　移動用制御点
	std::unique_ptr<RailManager> threeComboRailManager_; 
	std::unique_ptr<RailManager> stopPunchRailManager_; 


	Vector3 combo3MovePos_;
	
	
public:
	PlayerHandRight();
	///===========================================
	/// public method
	///=============================================

	void Init()override;
	void Update()override;
	void Draw(const ViewProjection& viewprojection)override;

	void RailThreeComboUpdate(const float& speed);
	void RailForthComboUpdate(const float& speed);
	void AdjustParm()override;
	void SaveAndLoad()override;

	///===========================================
	/// getter method
	///=============================================
	RailManager* GetThreeComboRailManager() { return threeComboRailManager_.get(); }
	RailManager* GetStopRailManager() { return stopPunchRailManager_.get(); }

	///===========================================
	/// setter method
	///=============================================
	void SetRailParent(WorldTransform* parent)override;
	void SetParent(WorldTransform* parent)override;
};