#pragma once
#include "BaseObject3d.h"

class Object3d : public BaseObject3d {
public:
    Object3d()  = default;
    ~Object3d()override;

    /// ============================================================
    /// public method
    /// ============================================================

    /// モデル作成
    static Object3d* CreateModel(const std::string& instanceName);

    /// 初期化、更新、描画
    void Init();
    void Update();
    void Draw(const ViewProjection& viewProjection);

    void DebugImgui() override;
    void ShadowDraw(const ViewProjection& viewProjection);

private:
    void UpdateWVPData(const ViewProjection& viewProjection) override;
    void CreateShadowMap() override;
    void CreateWVPResource() override;
    void CreateMaterialResource() override;
};