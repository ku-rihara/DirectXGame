#pragma once

#include "3d/ViewProjection.h"
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

struct RibbonVertex {
    Vector3 pos;
    Vector4 color;
    Vector2 uv;
};

struct RibbonCBuffer {
    Matrix4x4 viewProjection;
};

struct RibbonDistortionCBuffer {
    float strength;
    float pad[3]; // 16byte アライン
};

/// <summary>
/// リボントレイル描画クラス
/// </summary>
class RibbonTrail {
public:
    RibbonTrail()  = default;
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

    ///========================================================
    /// Setter（RibbonTrailPlayerから設定）
    ///========================================================

    /// 末端の色（head→tail グラデーション）
    void SetEndColor(const Vector4& color) { endColor_ = color; }
    /// 末端の幅（head→tail テーパー）
    void SetEndWidth(float width) { endWidth_ = width; }
    /// テクスチャをパスで設定（空文字列でデフォルト白）
    void SetTexture(const std::string& texturePath);

    /// 時空歪み設定
    void SetUseDistortion(bool use) { useDistortion_ = use; }
    void SetDistortionStrength(float strength) { distortionStrength_ = strength; }
    void SetDistortionTexture(const std::string& texturePath);

    /// 時空歪みパス描画（DrawDistortionPipelineがセット済みの状態で呼ぶ）
    void DrawDistortion(const ViewProjection& viewProj);

    ///========================================================
    /// Getter
    ///========================================================
    size_t GetPointCount()      const { return points_.size(); }
    bool   IsDistortionEnabled() const { return useDistortion_; }

private:
    void Init(size_t maxPoints);

    struct TrailPoint {
        Vector3 position;
        Vector4 startColor; // AddPoint 時のヘッド色
        float   startWidth; // AddPoint 時のヘッド幅
        float   lifetime;
        float   age = 0.0f;
    };


    static Vector3 CalcPerp(const Vector3& dir, const Vector3& cameraRight);

    static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

private:
    size_t maxPoints_ = 0;

    // グラデーション・テーパー（全ポイント共通、末端設定値）
    Vector4 endColor_ = {1.0f, 1.0f, 1.0f, 0.0f};
    float   endWidth_ = 0.0f;

    // テクスチャ
    uint32_t textureHandle_                           = UINT32_MAX;
    static constexpr const char* kDefaultTexturePath = "Resources/Texture/default.dds";

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW               vertexBufferView_{};
    RibbonVertex*                          vertexData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
    RibbonCBuffer*                         cBufferData_ = nullptr;

    // 時空歪み
    bool     useDistortion_       = false;
    float    distortionStrength_  = 0.1f;
    uint32_t distortionTextureHandle_ = UINT32_MAX;

    Microsoft::WRL::ComPtr<ID3D12Resource> distortionConstantBufferResource_;
    RibbonDistortionCBuffer*               distortionCBufferData_ = nullptr;

    size_t lastVertexCount_ = 0; // Draw() が書き込んだ頂点数（DrawDistortionで再利用）

    std::deque<TrailPoint> points_;
};

} // namespace KetaEngine
