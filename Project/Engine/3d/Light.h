#pragma once
#include <memory>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
//struct
#include"struct/DirectionalLight.h"
#include "struct/PointLight.h"
#include "struct/CameraForGPU.h"
#include "struct/SpotLight.h"

class Light {
private:
	//平行光源
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//虚面反射
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
	//ポイントライト
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	//スポットライト
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;

	//平行光源データ
	DirectionalLight* directionalLightData_;
	//鏡面反射用データ
	CameraForGPU* cameraForGPUData_;
	//ポイントライトデータ
	PointLight* pointLightData_;
	//スポットライトデータ
	SpotLight* spotLightData_;
public:
	static Light* GetInstance();
	void Init();
	void DebugImGui();

	//getter
	ID3D12Resource* GetDirectionalLightResource()const { return directionalLightResource_.Get(); }
	ID3D12Resource* GetCameraForGPUResource()const { return cameraForGPUResource_.Get(); }
	ID3D12Resource* GetPointLightResource()const { return pointLightResource_.Get(); }
	ID3D12Resource* GetSpotLightResource()const { return spotLightResource_.Get(); }

	void SetPointLightPos(const Vector3& pos);
	void SetSpotLightPos(const Vector3& pos);
	void SetWorldCameraPos(const Vector3& pos);
};