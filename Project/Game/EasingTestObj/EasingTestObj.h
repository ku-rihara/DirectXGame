#pragma once
#include "3d/Object3d.h"
#include "3d/WorldTransform.h"
#include "Easing/Easing.h"
#include "Easing/EasingSequence.h"

/// <summary>
/// イージングテスト用オブジェクトクラス
/// </summary>
class EasingTestObj {
private:
    enum class AdaptType {
        Position, // 位置
        Rotation, // 回転
        Scale, // 拡大縮小
    };

public:
    EasingTestObj();
    ~EasingTestObj();

    // 初期化、更新
    void Init();
    void Update();

    void Debug(); //< デバッグ処理
    void ScaleEasing(); //< スケールイージング

private:
    // モデル
    std::unique_ptr<Object3d> object3D_ = nullptr;

    Easing<Vector3> easing_;

public:
    Easing<Vector3>& GetEasingData() { return easing_; }
};