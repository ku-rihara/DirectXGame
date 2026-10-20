#pragma once

#include <memory>

namespace KetaEngine {

class Audio;
class AudioDSPEditor;

/// <summary>
/// AudioDSPEditorの標準構成(Audio実装・MP3書き出し・全パネル)を組み立てる
/// </summary>
class AudioDSPEditorFactory {
public:
    std::unique_ptr<AudioDSPEditor> Create(Audio& audio) const;
};

}; // KetaEngine
