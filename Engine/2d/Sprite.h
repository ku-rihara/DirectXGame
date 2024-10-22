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
public:

	struct UVTransform {
		Vector2 scale;
		Vector3 rotate;
		Vector2 pos;
	};
	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};
	Transform transform_;
	UVTransform uvTransform_;
	
private:
	
	//テクスチャ
	D3D12_GPU_DESCRIPTOR_HANDLE texture_;

	static D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	static D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	//リソース******************************************************************
	//Material
	//Microsoft::WRL::ComPtr<ID3D12Resource >materialResourceSprite_;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_;
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	//indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>indexResource_;
	//データ****************************************************************************
	TransformationMatrix2D* wvpData_;
	Material material_;


public:
	//シングルトンインスタンスの取得
	//static Sprite* GetInstance();
	static Sprite* Create(const uint32_t& textureHandle, const Vector2& position, const Vector4& color);
	/// <summary>
	/// スプライトの作成
	/// </summary>
	void CreateSprite(const uint32_t& textureHandle, const Vector2& position, const Vector4& color);

	//描画前
static	void PreDraw(ID3D12GraphicsCommandList* commandList);

//描画
	void Draw();

	//ポジションセット
	void SetPosition(const Vector2& pos);
	//スプライトセット
	void SetScale(const Vector2& scale);

	//UVセット
	void SetUVTransform();

#ifdef _DEBUG
	void DebugImGui();
#endif

	//setter
	void SetTransformationMatrixDataSprite(Matrix4x4 date) { this->wvpData_->WVP = date; }
	void SetWorldMatrixDataSprite(Matrix4x4 date) { this->wvpData_->World = date; }
	void SetUVTransformSprite(Matrix4x4 matrix) { this->material_.materialData_->uvTransform = matrix; }
};

