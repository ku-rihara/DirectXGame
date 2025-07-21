#include "DxCommand.h"
#include <cassert>

void DxCommand::Init(Microsoft::WRL::ComPtr<ID3D12Device> device) {
    CreateCommandQueue(device);
    CreateCommandAllocator(device);
    CreateCommandList(device);
    CreateFence(device);
}

void DxCommand::CreateCommandQueue(Microsoft::WRL::ComPtr<ID3D12Device> device) {
    commandQueue_ = nullptr;
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
    hr_ = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
    assert(SUCCEEDED(hr_));
}

void DxCommand::CreateCommandAllocator(Microsoft::WRL::ComPtr<ID3D12Device> device) {
    commandAllocator_ = nullptr;
    hr_               = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
    assert(SUCCEEDED(hr_));
}

void DxCommand::CreateCommandList(Microsoft::WRL::ComPtr<ID3D12Device> device) {
    commandList_ = nullptr;
    hr_          = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                 commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
    assert(SUCCEEDED(hr_));
}

void DxCommand::CreateFence(Microsoft::WRL::ComPtr<ID3D12Device> device) {
    fence_      = nullptr;
    fenceValue_ = 0;
    hr_         = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    assert(SUCCEEDED(hr_));

    fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent_ != nullptr);
}

void DxCommand::ExecuteCommand() {
    hr_ = commandList_->Close();
    assert(SUCCEEDED(hr_));

    Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = {commandList_.Get()};
    commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());

    // フェンス値を更新してシグナル送信
    fenceValue_++;
    commandQueue_->Signal(fence_.Get(), fenceValue_);
}

void DxCommand::ResetCommand() {
    hr_ = commandAllocator_->Reset();
    assert(SUCCEEDED(hr_));
    hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
    assert(SUCCEEDED(hr_));
}

void DxCommand::WaitForGPU() {
    if (fence_->GetCompletedValue() < fenceValue_) {
        fenceEvent_ = CreateEvent(nullptr, false, false, nullptr);
        fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
        CloseHandle(fenceEvent_);
    }
}

void DxCommand::Finalize() {
    if (fence_) {
        fence_.Reset();
    }
    if (commandList_) {
        commandList_.Reset();
    }
    if (commandAllocator_) {
        commandAllocator_.Reset();
    }
    if (commandQueue_) {
        commandQueue_.Reset();
    }
    if (fenceEvent_) {
        CloseHandle(fenceEvent_);
    }
}