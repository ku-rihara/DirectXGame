#pragma once

#include "Editor/ParameterEditor/GlobalParameter.h"
#include <memory>
#include <string>

/// <summary>
/// エフェクトデータクラス
/// </summary>
namespace KetaEngine {

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
    virtual void Update(float speedRate = 1.0f) = 0;
    virtual void Reset()                        = 0;
    virtual void Play()                         = 0;

    // カテゴリー初期化
    virtual void Init(const std::string& name, const std::string& categoryName);

    // ポーズ
    void Pause();

    // 再生状態取得
    bool IsPlaying() const;
    bool IsFinished() const;
    bool IsReturning() const;

    // ファイル操作
    virtual void LoadData();
    virtual void SaveData();

protected:
    //*---------------------------- protected Methods ----------------------------*//

    virtual void RegisterParams() = 0;
    virtual void GetParams()      = 0;
    virtual void InitParams()     = 0;

protected:
    //*---------------------------- protected Variant ----------------------------*//

    GlobalParameter* globalParameter_ = nullptr;
    std::string groupName_;
    std::string categoryName_;
    std::string folderPath_;
    PlayState playState_ = PlayState::STOPPED;
    bool showControls_   = true;

public:
    //*---------------------------- getter Methods ----------------------------*//

    const std::string& GetGroupName() const { return groupName_; }
    const std::string& GetCategoryName() const { return categoryName_; }
    PlayState GetPlayState() const { return playState_; }
};

}; // KetaEngine