#pragma once
#include "AreaLight.h"
#include <memory>
#include <vector>

/// <summary>
/// エリアライト管理クラス
/// </summary>
class AreaLightManager {
private:
    std::vector<std::unique_ptr<AreaLight>> areaLights_;

public:
    AreaLightManager()  = default;
    ~AreaLightManager() = default;

    ///=========================================================================================
    ///  public  method
    ///=========================================================================================

    /// <summary>
    /// エリアライトを追加
    /// </summary>
    /// <param name="device">デバイス</param>
    void Add(ID3D12Device* device);

    /// <summary>
    /// エリアライトを削除
    /// </summary>
    /// <param name="index">インデックス</param>
    void Remove(int index);

    /// <summary>
    /// ライトコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);

    void DebugImGui(); //< ImGuiデバッグ表示

public:
    ///=========================================================================================
    ///  getter  method
    ///=========================================================================================
    std::vector<AreaLight*> GetLights();
    AreaLight* GetAreaLight(int num) { return areaLights_[num].get(); }
};