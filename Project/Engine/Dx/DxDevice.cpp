#include "DxDevice.h"

using namespace KetaEngine;
#include "function/Convert.h"
#include "function/Log.h"
#include <cassert>
#include <format>

void DxDevice::Init() {
    CreateFactory();
    CreateDevice();
    SetupDebugLayer();
    Log("Complete create D3D12Device!!!\n");
}

void DxDevice::CreateFactory() {
    hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
    assert(SUCCEEDED(hr_));
}

void DxDevice::CreateDevice() {
    // 使用するアダプタを選択
    useAdapter_ = nullptr;
    for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
                         DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_))
                     != DXGI_ERROR_NOT_FOUND;
        ++i) {

        DXGI_ADAPTER_DESC3 adapterDesc{};
        hr_ = useAdapter_->GetDesc3(&adapterDesc);
        assert(SUCCEEDED(hr_));

        // ソフトウェアアダプタでなければ採用
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
            Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
            break;
        }
        useAdapter_ = nullptr;
    }
    assert(useAdapter_ != nullptr);

    // D3D12デバイス作成
    device_                           = nullptr;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0};
    const char* featureLevelStrings[] = {"12.2", "12.1", "12.0"};

    for (size_t i = 0; i < _countof(featureLevels); ++i) {
        hr_ = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
        if (SUCCEEDED(hr_)) {
            Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
            break;
        }
    }
    assert(device_ != nullptr);
}

void DxDevice::SetupDebugLayer() {
#ifdef _DEBUG
    ID3D12InfoQueue* infoQueue = nullptr;
    if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
        // エラーレベルでブレイクポイントを設定
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

        // 特定のメッセージを抑制
        D3D12_MESSAGE_ID denyIds[] = {
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE};
        D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
        D3D12_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumIDs        = _countof(denyIds);
        filter.DenyList.pIDList       = denyIds;
        filter.DenyList.NumSeverities = _countof(severities);
        filter.DenyList.pSeverityList = severities;

        infoQueue->PushStorageFilter(&filter);
        infoQueue->Release();
    }
#endif
}

void DxDevice::Finalize() {
    if (device_) {
        device_.Reset();
    }
    if (useAdapter_) {
        useAdapter_.Reset();
    }
    if (dxgiFactory_) {
        dxgiFactory_.Reset();
    }
}
