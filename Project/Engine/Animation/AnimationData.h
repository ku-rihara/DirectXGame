#pragma once

#include"Vector3.h"
#include"Quaternion.h"
#include<vector>
#include<string>
#include <map>

template<typename tValue>
struct Keyframe {
    float time;
    tValue value;
};
using KeyframeVector3=Keyframe<Vector3>;
using KeyframeQuaternion=Keyframe<Quaternion>;

template<typename tValue>
struct AnimationCurve {
    std::vector<Keyframe<tValue>> keyframes;
};

struct NodeAnimation {
    AnimationCurve<Vector3> translate;
    AnimationCurve<Quaternion> rotate;
    AnimationCurve<Vector3> scale;
};

struct Animation {
    float duration;
    std::string name;
    std::map<std::string, NodeAnimation> nodeAnimations;
};