#include "AudioPCM.h"

using namespace KetaEngine;
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>

AudioPCM::AudioPCM(int sampleRate, int channelCount, size_t length) {
    Allocate(sampleRate, channelCount, length);
}

void AudioPCM::Allocate(int sampleRate, int channelCount, size_t length) {
    sampleRate_ = sampleRate;
    channels_.assign(static_cast<size_t>((std::max)(channelCount, 1)), std::vector<double>(length, 0.0));
}

void AudioPCM::Resize(size_t length) {
    for (auto& ch : channels_) {
        ch.resize(length, 0.0);
    }
}

void AudioPCM::Clear() {
    for (auto& ch : channels_) {
        std::fill(ch.begin(), ch.end(), 0.0);
    }
}

void AudioPCM::ApplyGain(double gain) {
    for (auto& ch : channels_) {
        for (double& s : ch) {
            s *= gain;
        }
    }
}

void AudioPCM::Normalize(double peak) {
    double maxAmp = 0.0;
    for (const auto& ch : channels_) {
        for (double s : ch) {
            maxAmp = (std::max)(maxAmp, std::abs(s));
        }
    }
    if (maxAmp > 0.0) {
        ApplyGain(peak / maxAmp);
    }
}

void AudioPCM::ApplyFade(double fadeInSec, double fadeOutSec) {
    const size_t length  = GetLength();
    const size_t fadeIn  = (std::min)(length, static_cast<size_t>((std::max)(fadeInSec, 0.0) * sampleRate_));
    const size_t fadeOut = (std::min)(length, static_cast<size_t>((std::max)(fadeOutSec, 0.0) * sampleRate_));

    for (auto& ch : channels_) {
        for (size_t n = 0; n < fadeIn; ++n) {
            ch[n] *= static_cast<double>(n) / fadeIn;
        }
        for (size_t n = 0; n < fadeOut; ++n) {
            ch[length - n - 1] *= static_cast<double>(n) / fadeOut;
        }
    }
}

void AudioPCM::MixFrom(const AudioPCM& other, size_t destOffset, double gain) {
    if (other.channels_.empty() || destOffset >= GetLength()) {
        return;
    }
    const size_t count = (std::min)(other.GetLength(), GetLength() - destOffset);
    for (size_t c = 0; c < channels_.size(); ++c) {
        const auto& src = other.channels_[c % other.channels_.size()];
        for (size_t n = 0; n < count; ++n) {
            channels_[c][destOffset + n] += src[n] * gain;
        }
    }
}

std::vector<double> AudioPCM::GetMonoMix() const {
    std::vector<double> result(GetLength(), 0.0);
    if (channels_.empty()) {
        return result;
    }
    for (const auto& ch : channels_) {
        for (size_t n = 0; n < result.size(); ++n) {
            result[n] += ch[n];
        }
    }
    for (double& s : result) {
        s /= static_cast<double>(channels_.size());
    }
    return result;
}

bool AudioPCM::FromBytes(const WAVEFORMATEX& wfex, const std::vector<BYTE>& buffer) {
    // フォーマット判定(EXTENSIBLEの場合はサブフォーマットを見る)
    bool isFloat = wfex.wFormatTag == WAVE_FORMAT_IEEE_FLOAT;
    bool isPCM   = wfex.wFormatTag == WAVE_FORMAT_PCM;
    if (wfex.wFormatTag == WAVE_FORMAT_EXTENSIBLE && wfex.cbSize >= 22) {
        const auto& ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE&>(wfex);
        // サブフォーマットGUIDのData1にフォーマットタグが入っている
        isFloat = ext.SubFormat.Data1 == WAVE_FORMAT_IEEE_FLOAT;
        isPCM   = ext.SubFormat.Data1 == WAVE_FORMAT_PCM;
    }

    const int channelCount   = wfex.nChannels;
    const int bytesPerSample = wfex.wBitsPerSample / 8;
    if (channelCount <= 0 || bytesPerSample <= 0 || (!isFloat && !isPCM) || (isFloat && bytesPerSample != 4)) {
        return false;
    }

    const size_t frameSize = static_cast<size_t>(channelCount) * bytesPerSample;
    const size_t length    = buffer.size() / frameSize;
    Allocate(static_cast<int>(wfex.nSamplesPerSec), channelCount, length);

    const BYTE* p = buffer.data();
    for (size_t n = 0; n < length; ++n) {
        for (int c = 0; c < channelCount; ++c) {
            double value = 0.0;
            if (isFloat) {
                float f;
                std::memcpy(&f, p, sizeof(float));
                value = f;
            } else {
                switch (bytesPerSample) {
                case 1: // 8bitは符号なし
                    value = (static_cast<double>(p[0]) - 128.0) / 128.0;
                    break;
                case 2: {
                    int16_t s;
                    std::memcpy(&s, p, sizeof(int16_t));
                    value = s / 32768.0;
                    break;
                }
                case 3: {
                    int32_t s = (static_cast<int32_t>(p[0]) << 8) | (static_cast<int32_t>(p[1]) << 16) | (static_cast<int32_t>(p[2]) << 24);
                    value     = (s >> 8) / 8388608.0;
                    break;
                }
                case 4: {
                    int32_t s;
                    std::memcpy(&s, p, sizeof(int32_t));
                    value = s / 2147483648.0;
                    break;
                }
                default:
                    return false;
                }
            }
            channels_[c][n] = value;
            p += bytesPerSample;
        }
    }
    return true;
}

void AudioPCM::ToBytes16(WAVEFORMATEX& outFormat, std::vector<BYTE>& outBuffer) const {
    const int channelCount = (std::max)(GetChannelCount(), 1);

    outFormat                 = {};
    outFormat.wFormatTag      = WAVE_FORMAT_PCM;
    outFormat.nChannels       = static_cast<WORD>(channelCount);
    outFormat.nSamplesPerSec  = static_cast<DWORD>(sampleRate_);
    outFormat.wBitsPerSample  = 16;
    outFormat.nBlockAlign     = static_cast<WORD>(channelCount * 2);
    outFormat.nAvgBytesPerSec = outFormat.nSamplesPerSec * outFormat.nBlockAlign;
    outFormat.cbSize          = 0;

    const size_t length = GetLength();
    outBuffer.resize(length * outFormat.nBlockAlign);

    BYTE* p = outBuffer.data();
    for (size_t n = 0; n < length; ++n) {
        for (int c = 0; c < channelCount; ++c) {
            const double value = channels_.empty() ? 0.0 : std::clamp(channels_[c][n], -1.0, 1.0);
            const int16_t s    = static_cast<int16_t>(std::lround(value * 32767.0));
            std::memcpy(p, &s, sizeof(int16_t));
            p += sizeof(int16_t);
        }
    }
}

bool AudioPCM::SaveWav(const std::string& filePath) const {
    WAVEFORMATEX format;
    std::vector<BYTE> data;
    ToBytes16(format, data);

    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const uint32_t dataSize = static_cast<uint32_t>(data.size());
    const uint32_t fmtSize  = 16;
    const uint32_t riffSize = 4 + (8 + fmtSize) + (8 + dataSize);

    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&riffSize), 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    file.write(reinterpret_cast<const char*>(&fmtSize), 4);
    file.write(reinterpret_cast<const char*>(&format), fmtSize); // WAVEFORMATEXの先頭16byte(cbSize以外)
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);
    file.write(reinterpret_cast<const char*>(data.data()), dataSize);
    return file.good();
}
