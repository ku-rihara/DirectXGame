#pragma once
#include "SpotLight.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/// <summary>
/// スポットライト管理クラス
/// </summary>
class SpotLightManager {
public:
    SpotLightManager()  = default;
    ~SpotLightManager() = default;

    ///=========================================================================================
    ///  public  method
    ///=========================================================================================

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    void Init(ID3D12Device* device);

    /// <summary>
    /// スポットライトを追加
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="number">番号</param>
    void Add(ID3D12Device* device, const int32_t number);

    /// <summary>
    /// スポットライトを削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void Remove(int index);

    /// <summary>
    /// ライトコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);

    void Update(); //< 更新処理
    void UpdateStructuredBuffer(); //< StructuredBufferの更新
    void AdJustParams(); //< パラメータ調整

private:
    std::vector<std::unique_ptr<SpotLight>> spotLights_;

    // Structured Buffer用のリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> structuredBufferResource_;
    SpotLightData* structuredBufferData_;
    uint32_t srvIndex_;

    std::string groupName_ = "SpotLight";

public:
    ///=========================================================================================
    ///  getter  method
    ///=========================================================================================

    SpotLight* GetSpotLight(const int& num) { return spotLights_[num].get(); }
    size_t GetLightCount() const { return spotLights_.size(); }
    uint32_t GetSrvIndex() const { return srvIndex_; }
    std::vector<SpotLight*> GetLights();
};