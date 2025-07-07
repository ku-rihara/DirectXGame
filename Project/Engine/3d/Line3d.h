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

class Line3D {
public:
    void Init();
    void SetLine(const Vector3& start, const Vector3& end, const Vector4& color);
    void Draw(ID3D12GraphicsCommandList* commandList, const ViewProjection& viewProj);
 
    //球の描画
    void DrawSphereWireframe(const Vector3& center, float radius, const Vector4& color);
    void DrawCubeWireframe(const Vector3& center, float size, const Vector4& color);

    void Reset();

private:
    const size_t kMaxLines_        = 5120;
    const size_t kVerticesPerLine_ = 2;
    size_t kMaxVertices_;

    std::vector<Vertex> vertices_{};
    size_t currentLineCount_ = 0;

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    Vertex* vertexData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
    CBuffer* cBufferData_ = nullptr;
};
