#pragma once
#include <vector>
#include<memory>
#include "AreaLight.h"

class AreaLightManager {
private:
	std::vector<std::unique_ptr<AreaLight>> areaLights_;
public:

	AreaLightManager() = default;
    ~AreaLightManager() = default;

	///=========================================================================================
	///  public  method
	///=========================================================================================

	void Add(ID3D12Device* device);
	void Remove(int index);
	void DebugImGui();

	void SetLightCommand(ID3D12GraphicsCommandList* commandList);

	   ///=========================================================================================
	   ///  getter  method
	   ///=========================================================================================
	std::vector<AreaLight*> GetLights();
	AreaLight* GetAreaLight(int num) { return areaLights_[num].get(); }

};