#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>
//struct
#include"TransformationMatrix.h"
#include"DirectionalLight.h"
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
	ID3D12Resource* materialResource_;
	ID3D12Resource* materialResourceSprite_;
	//平行光源
	ID3D12Resource* directionalLightResource_;
	ID3D12Resource* directionalLightResourceSprite_;
	//頂点リソース
	ID3D12Resource* vertexResource_;
	ID3D12Resource* vertexResourceSprite_;
	//wvpリソース
	ID3D12Resource* wvpResouce_;
	ID3D12Resource* wvpResourceSprite_;
	//indexリソース
	ID3D12Resource* indexResource_;
	ID3D12Resource* indexResourceSprite_;

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

	/// <summary>
	/// スプライトの作成
	/// </summary>
	void CreateSprite();

	void DrawSphere();
	
	void DrawSprite();

	void ReleaseMesh();

	//setter
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
	void SetTransformationMatrixDataSprite(Matrix4x4 date) { this->wvpDataSprite_->WVP = date; }
	void SetWorldMatrixDataSprite(Matrix4x4 date) { this->wvpDataSprite_->World = date; }
	void SetUVTransformSprite(Matrix4x4 matrix) { this->materialDateSprite_->uvTransform = matrix; }
};

