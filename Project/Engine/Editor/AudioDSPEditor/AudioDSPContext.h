#pragma once

#include "AudioDSPGui.h"
#include <cstdint>
#include <string>

namespace KetaEngine {

class AudioPCM;
class ISoundRegistry;
class IAudioEncoder;

/// <summary>
/// 各パネルが共有するエディターの状態とサービス
/// パネル同士は直接参照せず、このコンテキスト経由でやり取りする
/// </summary>
class AudioDSPContext {
public:
    AudioDSPContext(ISoundRegistry& registry, IAudioEncoder& encoder) : registry_(registry), encoder_(encoder) {}

    // 生成結果を登録してプレビュー対象にする
    bool SubmitResult(const std::string& soundName, const AudioPCM& pcm);

    // 現在の音量で再生
    void Play(const std::string& soundName);

    // プレビュー対象を変更(同名でも再読み込みさせる)
    void SelectPreview(const std::string& soundName);

private:
    ISoundRegistry& registry_;
    IAudioEncoder& encoder_;
    AudioDSPGui gui_;

    std::string status_;
    std::string previewName_;
    uint32_t previewRevision_ = 0;
    float playVolume_         = 0.7f;

public:
    ISoundRegistry& GetRegistry() const { return registry_; }
    IAudioEncoder& GetEncoder() const { return encoder_; }
    const AudioDSPGui& GetGui() const { return gui_; }

    const std::string& GetStatus() const { return status_; }
    void SetStatus(const std::string& status) { status_ = status; }

    const std::string& GetPreviewName() const { return previewName_; }
    uint32_t GetPreviewRevision() const { return previewRevision_; }

    float& GetPlayVolume() { return playVolume_; }
};

}; // KetaEngine
