#include "PlayerComboAttackTimelineData.h"
#include <algorithm>

void PlayerComboAttackTimelineData::Init() {
    // 初期化
    defaultTrackIndices_.fill(-1);
    addedTracks_.clear();
}

void PlayerComboAttackTimelineData::AddTrackInfo(const TrackInfo& info) {
    // トラックに追加
    addedTracks_.push_back(info);
}

void PlayerComboAttackTimelineData::RemoveTrackInfo(int32_t trackIndex) {
    // 選択したトラックを削除
    auto it = std::remove_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });
    addedTracks_.erase(it, addedTracks_.end());

    // 削除後のインデックスを調整
    for (auto& track : addedTracks_) {
        if (track.trackIndex > trackIndex) {
            track.trackIndex--;
        }
    }
}

PlayerComboAttackTimelineData::TrackInfo*
PlayerComboAttackTimelineData::FindTrackInfo(int32_t trackIndex) {
    // indexからトラックの検索
    auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });

    if (it != addedTracks_.end()) {
        return &(*it);
    }
    return nullptr;
}

bool PlayerComboAttackTimelineData::IsTrackTypeAlreadyAdded(TrackType type) const {
    // キャンセルタイムと先行入力は各分岐ごとに複数追加可能なので、
    // ここでは常にfalseを返す
    if (type == TrackType::CANCEL_TIME || type == TrackType::PRECEDE_INPUT) {
        return false;
    }

    // 追加されたトラックのチェック
    for (const auto& track : addedTracks_) {
        if (track.type == type) {
            return true;
        }
    }

    return false;
}

const char* PlayerComboAttackTimelineData::GetTrackTypeName(TrackType type) const {
    switch (type) {
    case TrackType::CAMERA_ACTION:
        return "カメラアクション";
    case TrackType::HIT_STOP:
        return "ヒットストップ";
    case TrackType::SHAKE_ACTION:
        return "シェイクアクション";
    case TrackType::POST_EFFECT:
        return "ポストエフェクト";
    case TrackType::PARTICLE_EFFECT:
        return "パーティクルエフェクト";
    case TrackType::AUDIO_ATTACK:
        return "攻撃音";
    case TrackType::CAMERA_ACTION_ON_HIT:
        return "カメラアクション (ヒット時)";
    case TrackType::HIT_STOP_ON_HIT:
        return "ヒットストップ (ヒット時)";
    case TrackType::SHAKE_ACTION_ON_HIT:
        return "シェイクアクション (ヒット時)";
    case TrackType::POST_EFFECT_ON_HIT:
        return "ポストエフェクト (ヒット時)";
    case TrackType::PARTICLE_EFFECT_ON_HIT:
        return "パーティクルエフェクト (ヒット時)";
    case TrackType::AUDIO_ATTACK_ON_HIT:
        return "攻撃ヒット音";
    case TrackType::OBJ_ANIM_HEAD:
        return "頭アニメーション";
    case TrackType::OBJ_ANIM_RIGHT_HAND:
        return "右手アニメーション";
    case TrackType::OBJ_ANIM_LEFT_HAND:
        return "左手アニメーション";
    case TrackType::OBJ_ANIM_MAIN_HEAD:
        return "メイン頭アニメーション";
    case TrackType::CANCEL_TIME:
        return "キャンセルタイム";
    case TrackType::PRECEDE_INPUT:
        return "先行入力";
    case TrackType::FINISH_WAIT_TIME:
        return "攻撃終了待機";
    default:
        return "不明";
    }
}

PlayerComboAttackTimelineData::TrackType
PlayerComboAttackTimelineData::GetTrackTypeFromIndex(int32_t trackIndex) const {

    //  -------------------------デフォルトトラックから検索------------------------- //

    // デフォルトトラックのtypeをindexから検索
    for (size_t i = 0; i < defaultTrackIndices_.size(); ++i) {

        // デフォルトトラックと一致するindexを探す
        if (defaultTrackIndices_[i] != trackIndex) {
            continue;
        }

        // 一致したindexがどのトラックか判定
        switch (static_cast<DefaultTrack>(i)) {
        case DefaultTrack::COLLISION:
            return TrackType::COUNT; // コリジョンは特定のTrackTypeを持たない
        case DefaultTrack::MOVE_EASING:
            return TrackType::COUNT; // 移動イージングは特定のTrackTypeを持たない
        case DefaultTrack::FINISH_WAIT:
            return TrackType::FINISH_WAIT_TIME;
        default:
            break;
        }
    }

    //  -------------------------追加トラックから検索------------------------- //

    // addedTracks_配列から、trackIndexが一致するものを探す
    auto it = std::find_if(addedTracks_.begin(), addedTracks_.end(),
        [trackIndex](const TrackInfo& info) {
            return info.trackIndex == trackIndex;
        });

    // そのTrackInfoが持つtypeを返す
    if (it != addedTracks_.end()) {
        return it->type;
    }

    return TrackType::COUNT; // エラー値として使用
}

std::string PlayerComboAttackTimelineData::GetDirectoryForTrackType(TrackType type) const {
    const std::string basePath = "Resources/GlobalParameter/";
    // 各演出データのファイルパスを取得
    switch (type) {
    case TrackType::CAMERA_ACTION:
    case TrackType::CAMERA_ACTION_ON_HIT:
        return basePath + "CameraAnimation/Common/Dates";
    case TrackType::HIT_STOP:
    case TrackType::HIT_STOP_ON_HIT:
        return basePath + "TimeScale/Common/Dates";
    case TrackType::SHAKE_ACTION:
    case TrackType::SHAKE_ACTION_ON_HIT:
        return basePath + "ShakeEditor/Common/Dates";
    case TrackType::POST_EFFECT:
    case TrackType::POST_EFFECT_ON_HIT:
        return basePath + "PostEffect";
    case TrackType::PARTICLE_EFFECT:
    case TrackType::PARTICLE_EFFECT_ON_HIT:
        return basePath + "Particle/Player/Dates";
    case TrackType::AUDIO_ATTACK:
    case TrackType::AUDIO_ATTACK_ON_HIT:
        return "Resources/Audio/";
    case TrackType::OBJ_ANIM_HEAD:
        return basePath + "ObjEaseAnimation/Player/Dates/";
    case TrackType::OBJ_ANIM_RIGHT_HAND:
        return basePath + "ObjEaseAnimation/RightHand/Dates/";
    case TrackType::OBJ_ANIM_LEFT_HAND:
        return basePath + "ObjEaseAnimation/LeftHand/Dates/";
    case TrackType::OBJ_ANIM_MAIN_HEAD:
        return basePath + "ObjEaseAnimation/MainHead/Dates/";
    default:
        return basePath;
    }
}

void PlayerComboAttackTimelineData::SetDefaultTrackIndex(DefaultTrack track, int32_t index) {
    // デフォルトトラックにindexをセット
    defaultTrackIndices_[static_cast<size_t>(track)] = index;
}

int32_t PlayerComboAttackTimelineData::GetDefaultTrackIndex(DefaultTrack track) const {
    // デフォルトトラックのindexを取得
    return defaultTrackIndices_[static_cast<size_t>(track)];
}

void PlayerComboAttackTimelineData::UpdateTrackIndicesAfterInsert(int32_t insertPosition, int32_t count) {
    // 挿入位置以降のトラックインデックスをcountだけ増加
    for (auto& track : addedTracks_) {
        // 新しく追加された分岐トラック以外で、挿入位置以降のものを調整
        if (track.type != TrackType::CANCEL_TIME && track.type != TrackType::PRECEDE_INPUT) {
            if (track.trackIndex >= insertPosition) {
                track.trackIndex += count;
            }
        }
    }
}