#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
//struct
#include"TransformationMatrix.h"
#include"DirectionalLight.h"
#include"ModelData.h"
#include"Material.h"

class TextureManager;
class Sprite {
private:
	int32_t TextureHandle_;
	TextureManager* textureManager_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_{};
	//リソース******************************************************************
	//Material
	Microsoft::WRL::ComPtr<ID3D12Resource >materialResourceSprite_;
	//平行光源
	//Microsoft::WRL::ComPtr<ID3D12Resource*> directionalLightResourceSprite_;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResourceSprite_;
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResourceSprite_;
	//indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>indexResourceSprite_;
	//データ****************************************************************************
	TransformationMatrix* wvpDataSprite_;
	Material* materialDateSprite_;

public:
	//シングルトンインスタンスの取得
	static Sprite* GetInstance();

	/// <summary>
	/// スプライトの作成
	/// </summary>
	void CreateSprite();

	void DrawSprite();

	//setter
	void SetTransformationMatrixDataSprite(Matrix4x4 date) { this->wvpDataSprite_->WVP = date; }
	void SetWorldMatrixDataSprite(Matrix4x4 date) { this->wvpDataSprite_->World = date; }
	void SetUVTransformSprite(Matrix4x4 matrix) { this->materialDateSprite_->uvTransform = matrix; }
};

