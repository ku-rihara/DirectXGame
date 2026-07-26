#pragma once

#include"Vector3.h"
#include"Quaternion.h"
#include<vector>
#include<string>
#include <map>

namespace KetaEngine {

/// <summary>
/// ある時刻(time)における値(value)を表すキーフレーム
/// </summary>
template<typename tValue>
struct Keyframe {
    float time;
    tValue value;
};
using KeyframeVector3=Keyframe<Vector3>;
using KeyframeQuaternion=Keyframe<Quaternion>;

/// <summary>
/// キーフレームの集合で構成される1つのアニメーションカーブ
/// </summary>
template<typename tValue>
struct AnimationCurve {
    std::vector<Keyframe<tValue>> keyframes;
};

/// <summary>
/// 1ノード分の平行移動・回転・スケールのアニメーションカーブ
/// </summary>
struct NodeAnimation {
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

/// <summary>
/// 全ノード分のアニメーションカーブと再生時間・名前をまとめた構造体
/// </summary>
struct Animation {
    float duration;
    std::string name;
    std::map<std::string, NodeAnimation> nodeAnimations;
};

}; // KetaEngine
