#pragma once
#include "BaseObject3d.h"

/// <summary>
/// 3Dオブジェクトクラス
/// </summary>
namespace KetaEngine {

class Object3d : public BaseObject3d {
public:
    Object3d() = default;
    ~Object3d() override;

    /// <summary>
    /// モデルを持つオブジェクトを作成
    /// </summary>
    /// <param name="instanceName">インスタンス名</param>
    /// <returns>作成されたObject3dのポインタ</returns>
    static Object3d* CreateModel(const std::string& instanceName);

    // 初期化、更新、描画
    void Init();
    void Update();
    void Draw(const ViewProjection& viewProjection);
    void ShadowDraw(const ViewProjection& viewProjection);

    // デバッグ描画
    void DebugImGui() override; 

private:
    void UpdateWVPData(const ViewProjection& viewProjection) override;
    void CreateShadowMap() override;
    void CreateWVPResource() override;
    void CreateMaterialResource() override;
};

}; // KetaEngine
