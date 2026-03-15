#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>
// struct
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector2.h"
#include "Vector3.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

struct CameraForGPU {
    Vector3 worldPosition_;
};

struct PlayerOcclusionData {
    Vector2 screenPos;    // プレイヤーのスクリーン座標
    float   depth;        // プレイヤーの深度値
    float   screenRadius; // 判定半径
    float   ditherAlpha;  // ディザ閾値
    int32_t enabled;      // 有効フラグ
    float   pad[2];
};

// ライトの個数データ
struct LightCountData {
    int pointLightCount;
    int spotLightCount;
};

// 前方宣言
class DirectionalLight;
class PointLightManager;
class SpotLightManager;
class AreaLightManager;
class AmbientLight;
class DirectXCommon;

/// <summary>
/// ライト統合管理クラス
/// </summary>
class Light {
public:
    static Light* GetInstance();
    ~Light() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// ライトコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetLightCommands(ID3D12GraphicsCommandList* commandList);

    /// <summary>
    /// ワールドカメラ座標を設定
    /// </summary>
    /// <param name="pos">座標</param>
    void SetWorldCameraPos(const Vector3& pos);

    /// <summary>
    /// プレイヤーオクルージョンデータを設定
    /// </summary>
    void SetPlayerOcclusion(const PlayerOcclusionData& data);

    /// <summary>
    /// スポットライトを削除
    /// </summary>
    /// <param name="num">番号</param>
    void RemoveSpotLight(int num);

    /// <summary>
    /// ポイントライトを削除
    /// </summary>
    /// <param name="num">番号</param>
    void RemovePointLight(int num);

    void Update(); //< 更新処理
    void InitAllLights(); //< 全ライト初期化
    void DebugImGui(); //< ImGuiデバッグ表示
    void AddSpotLight(); //< スポットライト追加
    void AddPointLight(); //< ポイントライト追加
    void RegisterParams(); //< パラメータバインド

private:
    DirectXCommon* dxCommon_;
    GlobalParameter* globalParameter_;
    const std::string groupName_ = "LightCount";

    int32_t spotLightCountMax_;
    int32_t current;

    // Lights
    std::unique_ptr<DirectionalLight> directionalLight_;
    std::unique_ptr<PointLightManager> pointLightManager_;
    std::unique_ptr<SpotLightManager> spotLightManager_;
    std::unique_ptr<AreaLightManager> areaLightManager_;
    std::unique_ptr<AmbientLight> ambientLight_;

    // 鏡面反射
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
    CameraForGPU* cameraForGPUData_;

    // プレイヤーオクルージョン
    Microsoft::WRL::ComPtr<ID3D12Resource> playerOcclusionResource_;
    PlayerOcclusionData* playerOcclusionData_;

    // LightsData
    Microsoft::WRL::ComPtr<ID3D12Resource> lightCountResource_;
    LightCountData* lightCountData_;

public:
    // getter
    PointLightManager* GetPointLightManager() { return pointLightManager_.get(); }
    SpotLightManager* GetSpotLightManager() { return spotLightManager_.get(); }
    AmbientLight* GetAmbientLight() { return ambientLight_.get(); }
    DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); }
    Vector3 GetWorldCameraPos() const { return cameraForGPUData_->worldPosition_; }
};

}; // KetaEngine
