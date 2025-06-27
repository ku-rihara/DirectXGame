#pragma once


#include"Player/Parts/BasePlayerHand.h"


class PlayerHandLeft:public BasePlayerHand {
private:
	
private:

	///===========================================
	/// private variant
	///=============================================
	
	std::string railGroupName_;
	std::unique_ptr<RailManager>trustRailManager_;
	
public:
    PlayerHandLeft()  = default;
    ~PlayerHandLeft()=default;
	///===========================================
	/// public method
	///=============================================

	void Init()override;
	void Update()override;
	void Draw(const ViewProjection& viewprojection)override;

	void RailForthComboUpdate(const float& speed);
	 void AdjustParm()override;
	 void SaveAndLoad()override;

	 ///===========================================
	/// getter method
	///=============================================
	 RailManager* GetThrustRailManager() { return trustRailManager_.get(); }

	 ///===========================================
	/// setter method
	///=============================================
	 void SetRailParent(WorldTransform* parent)override;
	  void SetParent(WorldTransform* parent)override;
};