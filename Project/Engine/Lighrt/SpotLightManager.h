#pragma once
#include <vector>
#include<memory>
#include "SpotLight.h"

class SpotLightManager {
private:
	std::vector<std::unique_ptr<SpotLight>> spotLights_;
public:

	SpotLightManager() = default;
    ~SpotLightManager() = default;

	///=========================================================================================
	///  public  method
	///=========================================================================================

	void Add(ID3D12Device* device);
	void Remove(int index);

	void DebugImGui();

	std::vector<SpotLight*> GetLights();
	void SetLightCommand(ID3D12GraphicsCommandList* commandList);


	   ///=========================================================================================
	   ///  getter  method
	   ///=========================================================================================

	SpotLight* GetSpotLight(int num) { return spotLights_[num].get(); }

};