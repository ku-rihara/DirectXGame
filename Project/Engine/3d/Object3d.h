#pragma once
#include "BaseObject3d.h"

/// <summary>
/// 3Dオブジェクトクラス
/// </summary>
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

    void Init(); //< 初期化

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void Update(const float& deltaTime);

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Draw(const ViewProjection& viewProjection);

    void DebugImgui() override; //< ImGuiデバッグ表示

    /// <summary>
    /// シャドウ描画
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void ShadowDraw(const ViewProjection& viewProjection);

private:
    void UpdateWVPData(const ViewProjection& viewProjection) override;
    void CreateShadowMap() override;
    void CreateWVPResource() override;
    void CreateMaterialResource() override;
};