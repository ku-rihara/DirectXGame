#pragma once

namespace KetaEngine {

class AudioDSPContext;

/// <summary>
/// AudioDSPエディターの1セクション
/// </summary>
class IAudioDSPPanel {
public:
    virtual ~IAudioDSPPanel() = default;

    virtual const char* GetTitle() const = 0;
    virtual void Draw(AudioDSPContext& context) = 0;

    // 最初から開いておくか
    virtual bool IsDefaultOpen() const { return false; }
};

}; // KetaEngine
