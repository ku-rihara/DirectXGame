#pragma once
#include "3d/ViewProjection.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <array>
#include <d3d12.h>
#include <vector>
#include <wrl.h>

struct Vertex {
    Vector3 pos;
    Vector4 color;
};

struct CBuffer {
    Matrix4x4 viewProjection;
};

/// <summary>
/// 3D空間でのライン描画クラス
/// </summary>
class Line3D {
public:
    Line3D() = default;
    ~Line3D();

    /// <summary>
    /// Line3Dオブジェクトを作成
    /// </summary>
    /// <param name="lineMaxNum">最大ライン数</param>
    /// <returns>作成されたLine3Dのポインタ</returns>
    static Line3D* Create(const size_t& lineMaxNum);

    /// <summary>
    /// ラインを設定
    /// </summary>
    /// <param name="start">開始点</param>
    /// <param name="end">終了点</param>
    /// <param name="color">色</param>
    void SetLine(const Vector3& start, const Vector3& end, const Vector4& color);

    /// <summary>
    /// 球のワイヤーフレーム描画
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="radius">半径</param>
    /// <param name="color">色</param>
    void SetSphereWireframe(const Vector3& center, const float& radius, const Vector4& color);

    /// <summary>
    /// 立方体のワイヤーフレーム描画
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="size">サイズ</param>
    /// <param name="color">色</param>
    void SetCubeWireframe(const Vector3& center, const Vector3& size, const Vector4& color);

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProj">ビュープロジェクション</param>
    void Draw(const ViewProjection& viewProj);

    void Reset(); //< ライン情報のリセット
private:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="lineMaxNum">最大ライン数</param>
    void Init(const size_t& lineMaxNum);

private:
    size_t maxLines_;                   //< 最大ライン数
    const size_t kVerticesPerLine_ = 2; //< 1ラインあたりの頂点数
    size_t kMaxVertices_;               //< 最大頂点数

    size_t currentLineCount_ = 0;    //< 現在のライン数

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_; //< 頂点バッファ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};         //< 頂点バッファビュー
    Vertex* vertexData_ = nullptr;                        //< 頂点データのポインタ

    Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_; //< 定数バッファ
    CBuffer* cBufferData_ = nullptr;                                //< 定数バッファデータ
};