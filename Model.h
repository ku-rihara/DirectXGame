#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

#include<string>
#include"Vector4.h"
#include"Vector3.h"
#include"Vector2.h"
//struct
#include"TransformationMatrix.h"
#include"DirectionalLight.h"
#include"ModelData.h"
#include"Material.h"
#include "MaterialData.h"

class Model{
private:
	ModelData modelData_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//リソース******************************************************************
	//Material
	ID3D12Resource* materialResource_;
	//平行光源
	ID3D12Resource* directionalLightResource_;
	//頂点リソース
	ID3D12Resource* vertexResource_;
	//wvpリソース
	ID3D12Resource* wvpResouce_;
	//indexリソース
	ID3D12Resource* indexResource_;

	//データ****************************************************************************
	TransformationMatrix* wvpDate_;

	Material* materialDate_;
	
	DirectionalLight* directionalLightData_;
	//後に消すかも
	bool useMonsterBall = true;
public:
	//シングルトンインスタンスの取得
	static Model* GetInstance();

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel();
	/// <summary>
	/// モデル描画
	/// </summary>
	void DrawModel();

#ifdef _DEBUG
	void DebugImGui();
#endif
	void ReleaseModel();

	//getter
	ModelData GetModelData()const { return modelData_; }

	//setter
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
};

