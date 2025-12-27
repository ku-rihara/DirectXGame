#pragma once
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include <cstdint>
#include <d3d12.h>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <wrl.h>

namespace KetaEngine {

struct ConstBufferDataWorldTransform {
    Matrix4x4 matWorld;
};

enum class BillboardType {
    XYZ, //< XYZ軸ビルボード
    X, //< X軸ビルボード
    Y, //< Y軸ビルボード
    Z, //< Z軸ビルボード
};

struct AdaptRotate {
    bool isX;
    bool isY;
    bool isZ;
};

enum class RotateOder {
    XYZ, //< オイラー角
    Quaternion, //< クォータニオン
};

class ObjEaseAnimationPlayer;
class Object3DAnimation;

/// <summary>
/// ワールド変換クラス
/// </summary>
class WorldTransform {
private:
    struct Transform {
        Vector3 scale = Vector3::OneVector();
        Vector3 rotation;
        Vector3 translation;
        Quaternion quaternion;
    };

public:
    WorldTransform();
    ~WorldTransform();

    // 初期化、更新
    void Init();
    void UpdateMatrix();

    /// <summary>
    /// 指定方向を向く
    /// </summary>
    /// <param name="direction">方向ベクトル</param>
    /// <returns>ワールド方向ベクトル</returns>
    Vector3 LookAt(const Vector3& direction) const;

    /// <summary>
    /// ビルボード行列更新
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    /// <param name="billboardAxis">ビルボード軸タイプ</param>
    /// <param name="adaptRotate">回転適用設定</param>
    void BillboardUpdateMatrix(const ViewProjection& viewProjection, const BillboardType& billboardAxis = BillboardType::XYZ, const AdaptRotate& adaptRotate = {false, false, false});

    /// <summary>
    /// 正面方向を軸としたStart地点からのoffsetを計算
    /// </summary>
    /// <param name="startPos"></param>
    /// <param name="offsetValue"></param>
    /// <returns></returns>
    Vector3 CalcForwardOffset(const Vector3& offsetValue) const;

    /// <summary>
    /// オブジェクトイージングアニメーション再生
    /// </summary>
    /// <param name="categoryName">カテゴリー名</param>
    /// <param name="animationName">アニメーション名</param>
    void PlayObjEaseAnimation(const std::string& categoryName, const std::string& animationName);

    /// <summary>
    /// オブジェクトイージングアニメーション停止
    /// </summary>
    void StopObjEaseAnimation();

    /// <summary>
    /// アニメーション更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateObjEaseAnimation();

private:
    void UpdateAffineMatrix(); //< アフィン行列更新
    void ClearParentJoint(); //< ペアレントジョイントクリア
    void UpdateMatrixWithJoint(); //< ジョイントで行列更新
    bool HasParentJoint() const; //< ペアレントジョイントを持つか

    void ApplyAnimationToTransform();
    void InitOffsetTransform();

    Vector3 ScaleCalc(bool isDirectScale);

public:
    Vector3 scale_ = Vector3::OneVector();
    Vector3 rotation_;
    Vector3 translation_;
    Quaternion quaternion_;

    Transform offsetTransform_;

    Matrix4x4 matWorld_; //< ワールド行列

    const WorldTransform* parent_ = nullptr;
    RotateOder rotateOder_        = RotateOder::XYZ;

private:
    const Object3DAnimation* parentAnimation_ = nullptr;
    int32_t parentJointIndex_                 = -1;
    std::string parentJointName_;

    Matrix4x4 billboardMatrix_;
    Matrix4x4 backToFrontMatrix_;

    bool isUseDirectScale_ = false;

    // オブジェクトイージングアニメーション
    std::unique_ptr<ObjEaseAnimationPlayer> objEaseAnimationPlayer_;

public:
    /// <summary>
    /// ペアレント設定
    /// </summary>
    /// <param name="parent">親のWorldTransform</param>
    void SetParent(const WorldTransform* parent);

    /// <summary>
    /// ペアレントジョイント設定
    /// </summary>
    /// <param name="animation">アニメーション</param>
    /// <param name="jointName">ジョイント名</param>
    void SetParentJoint(const Object3DAnimation* animation, const std::string& jointName);

    Vector3 GetLocalPos() const; //< ローカル座標取得
    Vector3 GetWorldPos() const; //< ワールド座標取得

    Vector3 GetRightVector() const; //< 右方向ベクトル取得
    Vector3 GetUpVector() const; //< 上方向ベクトル取得
    Vector3 GetForwardVector() const; //< 前方向ベクトル取得

    /// <summary>
    /// アニメーションプレイヤー取得
    /// </summary>
    ObjEaseAnimationPlayer* GetObjEaseAnimationPlayer() { return objEaseAnimationPlayer_.get(); }
};

}; // KetaEngine
