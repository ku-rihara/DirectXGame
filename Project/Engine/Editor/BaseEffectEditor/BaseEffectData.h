#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
#include <memory>
#include <string>

/// <summary>
/// エフェクトデータクラス
/// </summary>
class BaseEffectData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED,
        RETURNING
    };

public:
    virtual ~BaseEffectData() = default;

    //*---------------------------- public Methods ----------------------------*//

    // 純粋仮想関数
    virtual void Init(const std::string& name)         = 0;
    virtual void Update(const float& speedRate = 1.0f) = 0;
    virtual void Reset()                               = 0;

    // 共通実装
    virtual void Play();
    virtual void Pause();
    virtual bool IsPlaying() const;
    virtual bool IsFinished() const;

    // ファイル操作
    virtual void LoadData() = 0;
    virtual void SaveData() = 0;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    virtual void RegisterParams() = 0;
    virtual void GetParams()      = 0;
    virtual void InitParams()     = 0;

protected:
    //*---------------------------- protected Variant ----------------------------*//

    GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;
    std::string folderPath_;
    PlayState playState_ = PlayState::STOPPED;
    bool showControls_   = true;

public:
    //*---------------------------- getter Methods ----------------------------*//

    const std::string& GetGroupName() const { return groupName_; }
    PlayState GetPlayState() const { return playState_; }
};