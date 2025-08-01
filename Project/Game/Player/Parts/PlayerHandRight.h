#pragma once


#include"Player/Parts/BasePlayerHand.h"


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
    PlayerHandRight()  = default;
    ~PlayerHandRight()=default;
	///===========================================
	/// public method
	///=============================================

	void Init()override;
	void Update()override;

	void RailThreeComboUpdate(const float& speed);
	void RailForthComboUpdate(const float& speed);
	void AdjustParm()override;
	void SaveAndLoad()override;

	virtual void DissolveAdapt(const float& dissolve);

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