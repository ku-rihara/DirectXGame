#include "PCMConverter.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

bool PCMConverter::Decode(const WAVEFORMATEX& format, const std::vector<BYTE>& buffer, AudioPCM& outPCM) const {
    // フォーマット判定(EXTENSIBLEの場合はサブフォーマットを見る)
    bool isFloat = format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT;
    bool isPCM   = format.wFormatTag == WAVE_FORMAT_PCM;
    if (format.wFormatTag == WAVE_FORMAT_EXTENSIBLE && format.cbSize >= 22) {
        const auto& ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE&>(format);
        // サブフォーマットGUIDのData1にフォーマットタグが入っている
        isFloat = ext.SubFormat.Data1 == WAVE_FORMAT_IEEE_FLOAT;
        isPCM   = ext.SubFormat.Data1 == WAVE_FORMAT_PCM;
    }

    const int channelCount   = format.nChannels;
    const int bytesPerSample = format.wBitsPerSample / 8;
    if (channelCount <= 0 || bytesPerSample <= 0 || (!isFloat && !isPCM) || (isFloat && bytesPerSample != 4)) {
        return false;
    }

    const size_t frameSize = static_cast<size_t>(channelCount) * bytesPerSample;
    const size_t length    = buffer.size() / frameSize;
    outPCM.Allocate(static_cast<int>(format.nSamplesPerSec), channelCount, length);

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
                    const int32_t s = (static_cast<int32_t>(p[0]) << 8) | (static_cast<int32_t>(p[1]) << 16) | (static_cast<int32_t>(p[2]) << 24);
                    value           = (s >> 8) / 8388608.0;
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
            outPCM.GetChannel(c)[n] = value;
            p += bytesPerSample;
        }
    }
    return true;
}

void PCMConverter::Encode16(const AudioPCM& pcm, WAVEFORMATEX& outFormat, std::vector<BYTE>& outBuffer) const {
    const int channelCount = (std::max)(pcm.GetChannelCount(), 1);

    outFormat                 = {};
    outFormat.wFormatTag      = WAVE_FORMAT_PCM;
    outFormat.nChannels       = static_cast<WORD>(channelCount);
    outFormat.nSamplesPerSec  = static_cast<DWORD>(pcm.GetSampleRate());
    outFormat.wBitsPerSample  = 16;
    outFormat.nBlockAlign     = static_cast<WORD>(channelCount * 2);
    outFormat.nAvgBytesPerSec = outFormat.nSamplesPerSec * outFormat.nBlockAlign;
    outFormat.cbSize          = 0;

    const size_t length = pcm.GetLength();
    outBuffer.resize(length * outFormat.nBlockAlign);

    BYTE* p = outBuffer.data();
    for (size_t n = 0; n < length; ++n) {
        for (int c = 0; c < channelCount; ++c) {
            const double value = (pcm.GetChannelCount() == 0) ? 0.0 : std::clamp(pcm.GetChannel(c)[n], -1.0, 1.0);
            const int16_t s    = static_cast<int16_t>(std::lround(value * 32767.0));
            std::memcpy(p, &s, sizeof(int16_t));
            p += sizeof(int16_t);
        }
    }
}
