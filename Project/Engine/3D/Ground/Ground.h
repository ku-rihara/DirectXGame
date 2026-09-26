#pragma once
#include "3D/Object3d/Object3d.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Math/Vector3.h"
#include <memory>
#include <string>

namespace KetaEngine {

/// <summary>
/// 原点に置く地面
/// サイズはImGuiから変更でき、GlobalParameterで保存/読込できる
/// </summary>
class Ground {
public:
    Ground()  = default;
    ~Ground() = default;

    ///========================================================
    /// public method
    ///========================================================

    void Init(); //< 初期化
    void Update(); //< 更新
    void AdjustParam(); //< ImGuiでのパラメータ編集

private:
    ///========================================================
    /// private method
    ///========================================================

    void RegisterParams(); //< GlobalParameterへの登録

private:
    ///========================================================
    /// private variant
    ///========================================================

    GlobalParameter* globalParameter_ = nullptr;

    std::unique_ptr<Object3d> obj3d_ = nullptr;

    // 原点を中心に置く地面のサイズ(スケール)
    Vector3 size_ = Vector3::OneVector();

    const std::string kGroupName_ = "Ground";
    const std::string kModelName_ = "BackObj/Ground.obj";

public:
    ///========================================================
    /// getter method
    ///========================================================
    Object3d* GetObject3d() const { return obj3d_.get(); }
    const Vector3& GetSize() const { return size_; }

    ///========================================================
    /// setter method
    ///========================================================
    void SetSize(const Vector3& size) { size_ = size; }
};

} // namespace KetaEngine
