#pragma once
#include "3d/WorldTransform.h"
#include "Primitive/PrimitiveBox.h"
#include <memory>

/// <summary>
/// スカイボックス管理クラス
/// </summary>
class SkyBox {
public:
    SkyBox();
    ~SkyBox();

    // 初期化、更新、デバッグ
    void Init();
    void Update();
    void Debug(); 

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Draw(KetaEngine::ViewProjection& viewProjection);

private:
	// ワールド変換データ
    KetaEngine::WorldTransform transform_;
	// モデル
    std::unique_ptr<KetaEngine::PrimitiveBox> primitiveBox_ = nullptr;
};
