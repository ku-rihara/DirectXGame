#include "ObjectColor.h"

using namespace KetaEngine;
#include"Dx/DirectXCommon.h"
#include <cassert>
#include <intsafe.h>

// 初期化
void ObjectColor::Init() {
    // 定数バッファの生成
    CreateConstBuffer();

    // マッピングして、定数バッファのアドレスを取得
    Map();

    // 初期色を転送する
    TransferMatrix();
}

// 定数バッファ生成
void ObjectColor::CreateConstBuffer() {
    // DirectXCommon インスタンスを取得
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    assert(dxCommon);

    // 定数バッファのサイズは 256 バイトの倍数である必要がある
    size_t sizeCB = (sizeof(ConstBufferDataObjectColor) + 0xff) & ~0xff;

    // 定数バッファのリソースを生成
    constBuffer_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeCB);

    assert(constBuffer_);
}

// マッピング
void ObjectColor::Map() {
    // 定数バッファのマッピング
    HRESULT result = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
    assert(SUCCEEDED(result));

    if (FAILED(result)) {
        OutputDebugStringA("ConstBuffer Map failed.\n");
        return;
    }

    // 初期色の転送
    constMap_->color_ = color_;
}


// 行列を転送する
void ObjectColor::TransferMatrix() {
    if (constMap_) {
        // 色を定数バッファに転送
        constMap_->color_ = color_;
    }
}

// グラフィックスコマンドを積む
void ObjectColor::SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, const UINT& rootParameterIndex) const {
    assert(commandList);
    // 定数バッファビューを設定
    commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuffer_->GetGPUVirtualAddress());
}
