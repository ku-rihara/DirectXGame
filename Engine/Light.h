#pragma once
#include <memory>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
//struct
#include"DirectionalLight.h"
#include "PointLight.h"
#include "CameraForGPU.h"

class Light {
private:
	//平行光源
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//虚面反射
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
	//ポイントライト
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;

	//平行光源データ
	DirectionalLight* directionalLightData_;
	//鏡面反射用データ
	CameraForGPU* cameraForGPUData_;
	//ポイントライトデータ
	PointLight* pointLightData_;
public:
	static Light* GetInstance();
	void Init();
	void DebugImGui();

	//getter
	ID3D12Resource* GetDirectionalLightResource()const { return directionalLightResource_.Get(); }
	ID3D12Resource* GetCameraForGPUResource()const { return cameraForGPUResource_.Get(); }
	ID3D12Resource* GetPointLightResource()const { return pointLightResource_.Get(); }
};