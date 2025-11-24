#pragma once
#include "BaseEffectData.h"
#include <memory>
#include <vector>

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

    // 初期化、カテゴリー対応初期化
    void Init(const std::string& name) override;
    virtual void InitWithCategory(const std::string& name, const std::string& categoryName);
    // キーフレーム管理
    void AddKeyFrame();
    void RemoveKeyFrame(const int32_t& index);
    void ClearKeyFrames();
    void UpdateKeyFrameIndices();

    // キーフレームファイル操作
    void LoadData() override;
    void SaveData() override;
    virtual void LoadSequenceElements() = 0;
    virtual void SaveSequenceElements() = 0;

protected:
    //*---------------------------- protected Methods ----------------------------*//

    virtual void UpdateKeyFrameProgression()                                       = 0;
    virtual void AdvanceToNexTSequenceElement()                                    = 0;
    virtual std::unique_ptr<TSequenceElement> CreateKeyFrame(const int32_t& index) = 0;
    virtual std::string GeTSequenceElementFolderPath() const                       = 0;

protected:
    //*---------------------------- protected Variant ----------------------------*//
    std::vector<std::unique_ptr<TSequenceElement>> sectionElements_;
    int32_t activeKeyFrameIndex_   = 0;
    int32_t selectedKeyFrameIndex_ = -1;
    bool isAllKeyFramesFinished_   = false;
    std::string categoryName_;

public:
    //*----------------------------- getter Methods -----------------------------*//
    const int32_t& GetActiveKeyFrameIndex() const { return activeKeyFrameIndex_; }
    const int32_t& GetSelectedKeyFrameIndex() const { return selectedKeyFrameIndex_; }
    int32_t GetTotalKeyFrameCount() const { return static_cast<int32_t>(sectionElements_.size()); }
    const TSequenceElement* GetSelectedKeyFrame() const;

    //*----------------------------- setter Methods -----------------------------*//
    void SetSelectedKeyFrameIndex(const int32_t& index);
};