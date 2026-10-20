#pragma once

#include "IAudioEncoder.h"
#include "PCMConverter.h"
#include <string>

namespace KetaEngine {

/// <summary>
/// Media Foundation(Windows標準のMP3エンコーダー)でmp3を書き出す
/// 対応: 32000/44100/48000Hz、1～2ch
/// </summary>
class MP3Encoder : public IAudioEncoder {
public:
    struct Param {
        unsigned int bitrateKbps = 192; //< 128/160/192/224/256/320等
    };

public:
    MP3Encoder();
    explicit MP3Encoder(const Param& param);
    ~MP3Encoder() override;

    // コピー禁止(Media Foundationの初期化回数を管理するため)
    MP3Encoder(const MP3Encoder&)            = delete;
    MP3Encoder& operator=(const MP3Encoder&) = delete;

    bool Encode(const AudioPCM& pcm, const std::string& filePath) override;
    const char* GetExtension() const override { return ".mp3"; }
    const std::string& GetLastErrorMessage() const override { return lastError_; }

    // pcmがこのエンコーダーで書き出せる形式か
    bool IsSupported(const AudioPCM& pcm) const;

private:
    bool Fail(const std::string& message);

private:
    Param param_;
    PCMConverter converter_;
    bool isMediaFoundationStarted_ = false;
    std::string lastError_;

public:
    Param& GetParam() { return param_; }
    void SetParam(const Param& param) { param_ = param; }
};

}; // KetaEngine
