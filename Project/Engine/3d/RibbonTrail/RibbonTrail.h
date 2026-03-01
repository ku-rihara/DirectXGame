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

/// <summary>
/// リボントレイル描画クラス
/// 手や武器などに追従させ、軌跡をリボン状のメッシュで表現する
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
    /// 描画（マネージャーから呼ばれる）
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

    ///========================================================
    /// Getter
    ///========================================================
    size_t GetPointCount() const { return points_.size(); }

private:
    void Init(size_t maxPoints);

    struct TrailPoint {
        Vector3 position;
        Vector4 startColor; // AddPoint 時のヘッド色
        float   startWidth; // AddPoint 時のヘッド幅
        float   lifetime;
        float   age = 0.0f;
    };

    /// <summary>
    /// [Fix] cameraRight を受け取りビルボード対応の垂直ベクトルを返す
    /// 旧実装はワールド固定upベクトルを使っていたため、
    /// カメラ角度によってはトレイルが完全に厚み0の平面になって消えていた
    /// </summary>
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

    std::deque<TrailPoint> points_;
};

} // namespace KetaEngine
