#pragma once
#include "PointLight.h"
#include <memory>
#include <vector>

/// <summary>
/// ポイントライト管理クラス
/// </summary>
namespace KetaEngine {

class PointLightManager {
private:
    std::vector<std::unique_ptr<PointLight>> pointLights_;

    // Structured Buffer用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> structuredBufferResource_;
    PointLightData* structuredBufferData_;
    uint32_t srvIndex_;

public:
    PointLightManager()  = default;
    ~PointLightManager() = default;

    ///=========================================================================================
    ///  public  method
    ///=========================================================================================

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    void Init(ID3D12Device* device);

    /// <summary>
    /// ポイントライトを追加
    /// </summary>
    /// <param name="device">デバイス</param>
    void Add(ID3D12Device* device);

    /// <summary>
    /// ポイントライトを削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void Remove(int index);

    /// <summary>
    /// ライトコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);

    void UpdateStructuredBuffer(); //< StructuredBufferの更新
    void DebugImGui(); //< ImGuiデバッグ表示
    std::vector<PointLight*> GetLights();

    ///=========================================================================================
    ///  getter  method
    ///=========================================================================================

    PointLight* GetPointLight(int num) { return pointLights_[num].get(); }
    size_t GetLightCount() const { return pointLights_.size(); }
};

}; // KetaEngine
