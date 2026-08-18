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

/// <summary>
/// 鏡面反射計算用にGPUへ渡すカメラワールド座標
/// </summary>
struct CameraForGPU {
    Vector3 worldPosition_;
};

/// <summary>
/// 遮蔽物によるディザ抜き表示(オブジェクトがカメラに隠れた際のフェード)用のパラメータ
/// </summary>
struct DitherOcclusionData {
    Vector2 screenPos; // 対象のスクリーン座標
    float depth; // 対象の深度値
    float screenRadius; // 判定半径
    float ditherAlpha; // ディザ閾値
    int32_t enabled; // 有効フラグ
    float pad[2];
};

/// <summary>
/// ライトの個数データ
/// </summary>
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
private:
    Light()                        = default;
    ~Light()                       = default;
    // コピー禁止
    Light(const Light&)            = delete;
    Light& operator=(const Light&) = delete;

public:
    static Light* GetInstance();

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
    /// ディザオクルージョンデータを設定
    /// </summary>
    void SetDitherOcclusion(const DitherOcclusionData& data);

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

    /// <summary>
    /// シーン切替時に、ライト構成をそのシーン専用の保存データで作り直す
    /// </summary>
    /// <param name="sceneName">これから始まるシーンの名前</param>
    void OnSceneChanged(const std::string& sceneName);

    void Update(); //< 更新処理
    void InitAllLights(); //< 全ライト初期化
    void DebugImGui(); //< ImGuiデバッグ表示

    // ライトの追加・削除
    void AddSpotLight(); 
    void AddPointLight(); 
    // パラメータ登録
    void RegisterParams(); 
    void RegisterWorldCameraParams(); 

    void Finalize(); //< 終了処理

private:
    /// <summary>
    /// シーン名から保存先フォルダ名を生成する
    /// </summary>
    std::string MakeSceneFolder(const std::string& sceneName) const;

    /// <summary>
    /// 現在のシーン用にPoint/SpotLightを全て作り直す
    /// </summary>
    void RebuildLightsForScene(const std::string& sceneName);

private:
    DirectXCommon* dxCommon_;
    GlobalParameter* globalParameter_;
    const std::string groupName_           = "LightCount";
    const std::string worldCameraGroupName_ = "WorldCamera";

    int32_t spotLightCountMax_;
    int32_t pointLightCountMax_ = 0;
    int32_t current;

    // シーンごとのライト構成
    std::string currentSceneName_;   //< 現在ライトを構築済みのシーン名
    std::string pendingSceneName_;   //< 切り替える予定のシーン名

    // Lights
    std::unique_ptr<DirectionalLight> directionalLight_;
    std::unique_ptr<PointLightManager> pointLightManager_;
    std::unique_ptr<SpotLightManager> spotLightManager_;
    std::unique_ptr<AreaLightManager> areaLightManager_;
    std::unique_ptr<AmbientLight> ambientLight_;

    // 鏡面反射
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraForGPUResource_;
    CameraForGPU* cameraForGPUData_;
    Vector3 worldCameraPos_; 

    // ディザオクルージョン
    Microsoft::WRL::ComPtr<ID3D12Resource> ditherOcclusionResource_;
    DitherOcclusionData* ditherOcclusionData_;

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
