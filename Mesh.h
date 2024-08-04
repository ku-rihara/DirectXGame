#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
//struct
#include"TransformationMatrix.h"
#include"DirectionalLight.h"
#include"ModelData.h"
#include"Material.h"

class Mesh{
private:
	
	//球
	const uint32_t kSubdivision_ = 16;//分割数
	const uint32_t  shpereVertexNum_ = 1536;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
	//リソース******************************************************************
	//Material
	Microsoft::WRL::ComPtr<ID3D12Resource >materialResource_;
	//平行光源
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_;
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	//indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>indexResource_;
	//データ****************************************************************************
	TransformationMatrix* wvpDate_;
	TransformationMatrix* wvpDataSprite_;
	Material* materialDate_;
	Material* materialDateSprite_;
	DirectionalLight* directionalLightData_;
	//後に消すかも
	bool useMonsterBall = true;
public:
	//シングルトンインスタンスの取得
	static Mesh* GetInstance();

	/// <summary>
	/// 球の作成
	/// </summary>
	void CreateSphere();

	
//#ifdef _DEBUG
//	void DebugImGui();
//#endif

	//setter
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
};

