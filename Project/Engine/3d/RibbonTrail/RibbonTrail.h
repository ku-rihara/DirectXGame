#pragma once

#include "3D/ViewProjection.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cstdint>
#include <d3d12.h>
#include <deque>
#include <string>
#include <wrl.h>

namespace KetaEngine {

/// 弧の回転方向
enum class ArcDirection {
    CounterClockwise, ///< 左回転
    Clockwise, ///< 右回転
};

/// 弧を描く平面の基準軸
enum class ArcPlane {
    XZ, 
    XY, 
    YZ, 
};

struct RibbonVertex {
    Vector3 pos;
    Vector4 color;
    Vector2 uv;
};

struct RibbonCBuffer {
    Matrix4x4 viewProjection;
};

struct RibbonUVScrollCBuffer {
    Vector2 offset;
    float pad[2];
};

/// <summary>
/// リボントレイル描画クラス
/// </summary>
class RibbonTrail {
public:
    RibbonTrail() = default;
    ~RibbonTrail();

    /// <summary>
    /// RibbonTrailオブジェクトを作成しマネージャーに登録
    /// </summary>
    static RibbonTrail* Create(size_t maxPoints);

    /// <summary>
    /// トレイルにポイントを追加
    /// </summary>
    void AddPoint(const Vector3& position, const Vector4& startColor, float startWidth, float lifetime = 0.3f);

    /// <summary>
    /// 弧状にポイントを一括生成する
    /// </summary>
    void SetArc(
        const Vector3& basePos,
        float startAngle,
        float endAngle,
        float radius,
        ArcDirection direction = ArcDirection::CounterClockwise,
        ArcPlane plane         = ArcPlane::XZ,
        int segments           = -1);

    /// <summary>
    /// ポイントの時間経過更新と期限切れ削除
    /// </summary>
    void Update(float deltaTime);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw(const ViewProjection& viewProj);

    /// <summary>
    /// 全ポイントを即時クリア
    /// </summary>
    void Clear();

    /// 時空歪みパス描画
    void DrawDistortion(const ViewProjection& viewProj);

private:
    void Init(size_t maxPoints);

    /// <summary>
    ///  トレイルのポイントのパラメータ
    /// </summary>
    struct TrailPoint {
        Vector3 position;
        Vector4 startColor;
        float startWidth;
        float lifetime;
        float age = 0.0f;
    };

    static Vector3 CalcPerp(const Vector3& dir, const Vector3& cameraRight);
    static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

private:
    size_t maxPoints_ = 0;

    // グラデーション・テーパー
    Vector4 endColor_ = {1.0f, 1.0f, 1.0f, 0.0f};
    float endWidth_   = 0.0f;

    // テクスチャ
    uint32_t textureHandle_                          = UINT32_MAX;
    static constexpr const char* kDefaultTexturePath = "Resources/Texture/default.dds";

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    RibbonVertex* vertexData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
    RibbonCBuffer* cBufferData_ = nullptr;

    // 時空歪み
    bool useDistortion_               = false;
    float distortionStrength_         = 0.1f;
    uint32_t distortionTextureHandle_ = UINT32_MAX;

    // UVスクロール（メインテクスチャ）
    Vector2 uvScrollSpeed_  = Vector2::ZeroVector();
    Vector2 uvScrollOffset_ = Vector2::ZeroVector();

    Microsoft::WRL::ComPtr<ID3D12Resource> uvScrollCBufferResource_;
    RibbonUVScrollCBuffer* uvScrollCBufferData_ = nullptr;

    // UVスクロール（歪みテクスチャ）
    Vector2 distortionUVScrollSpeed_  = Vector2::ZeroVector();
    Vector2 distortionUVScrollOffset_ = Vector2::ZeroVector();

    Microsoft::WRL::ComPtr<ID3D12Resource> distortionUVScrollCBufferResource_;
    RibbonUVScrollCBuffer* distortionUVScrollCBufferData_ = nullptr;

    size_t lastVertexCount_ = 0;

    std::deque<TrailPoint> points_;

public:

    ///========================================================
    /// Getter
    ///========================================================
    size_t GetPointCount() const { return points_.size(); }
    bool IsDistortionEnabled() const { return useDistortion_; }

    ///========================================================
    /// Setter
    ///========================================================

    void SetEndColor(const Vector4& color) { endColor_ = color; }
    void SetEndWidth(float width) { endWidth_ = width; }
    void SetTexture(const std::string& texturePath);
    void SetUseDistortion(bool use) { useDistortion_ = use; }
    void SetDistortionStrength(float strength) { distortionStrength_ = strength; }
    void SetDistortionTexture(const std::string& texturePath);
    void SetUVScrollSpeed(const Vector2& speed) { uvScrollSpeed_ = speed; }
    void SetDistortionUVScrollSpeed(const Vector2& speed) { distortionUVScrollSpeed_ = speed; }
};

} // namespace KetaEngine
