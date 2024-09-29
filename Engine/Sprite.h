#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
//struct
#include"TransformationMatrix.h"
#include"DirectionalLight.h"
#include"ModelData.h"
#include"Material.h"


class Sprite {
private:
	//テクスチャ
	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	static D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite_;
	static D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite_;
	//リソース******************************************************************
	//Material
	Microsoft::WRL::ComPtr<ID3D12Resource >materialResourceSprite_;
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
	//static Sprite* GetInstance();

	/// <summary>
	/// スプライトの作成
	/// </summary>
	void CreateSprite(uint32_t textureHandle, Vector2 position, Vector4 color);

	//描画前
static	void PreDraw(ID3D12GraphicsCommandList* commandList);

//描画
	void DrawSprite(D3D12_GPU_DESCRIPTOR_HANDLE texutre);

	//ポジションセット
	void SetPosition(const Vector2& pos);


#ifdef _DEBUG
	void DebugImGui();
#endif

	//setter
	void SetTransformationMatrixDataSprite(Matrix4x4 date) { this->wvpDataSprite_->WVP = date; }
	void SetWorldMatrixDataSprite(Matrix4x4 date) { this->wvpDataSprite_->World = date; }
	void SetUVTransformSprite(Matrix4x4 matrix) { this->materialDateSprite_->uvTransform = matrix; }
};

