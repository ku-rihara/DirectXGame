#pragma once
#include "BaseEffectData.h"
#include <memory>
#include <vector>

namespace KetaEngine {

template <typename TSequenceElement>
class BaseSequenceEffectData;

/// <summary>
/// シークエンスエフェクトデータ基底クラス
/// </summary>
template <typename TSequenceElement>
class BaseSequenceEffectData : public BaseEffectData {
public:
    virtual ~BaseSequenceEffectData() = default;

    //*----------------------------- public Methods -----------------------------*//

    // カテゴリー対応初期化
    void Init(const std::string& name, const std::string& categoryName) override;

    // キーフレーム管理
    void AddKeyFrame();
    void RemoveKeyFrame(int32_t index);
    void ClearKeyFrames();
    void UpdateKeyFrameIndices();

    // キーフレームファイル操作
    void LoadData() override;
    void SaveData() override;
    virtual void LoadSequenceElements() = 0;
    virtual void SaveSequenceElements() = 0;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    virtual void UpdateKeyFrameProgression()                                = 0;
    virtual void AdvanceToNextSequenceElement()                             = 0;
    virtual std::unique_ptr<TSequenceElement> CreateKeyFrame(int32_t index) = 0;
    virtual std::string GetSequenceElementFolderPath() const                = 0;

protected:
    //*---------------------------- protected Variant ----------------------------*//
    std::vector<std::unique_ptr<TSequenceElement>> sectionElements_;
    int32_t activeKeyFrameIndex_   = 0;
    int32_t selectedKeyFrameIndex_ = -1;
    bool isAllKeyFramesFinished_   = false;

public:
    //*----------------------------- getter Methods -----------------------------*//
    int32_t GetActiveKeyFrameIndex() const { return activeKeyFrameIndex_; }
    int32_t GetSelectedKeyFrameIndex() const { return selectedKeyFrameIndex_; }
    int32_t GetTotalKeyFrameCount() const { return static_cast<int32_t>(sectionElements_.size()); }
    const TSequenceElement* GetSelectedKeyFrame() const;

    //*----------------------------- setter Methods -----------------------------*//
    void SetSelectedKeyFrameIndex(int32_t index);
};

}; // KetaEngine