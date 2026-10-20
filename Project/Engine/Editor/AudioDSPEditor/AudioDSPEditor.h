#pragma once

#include <memory>
#include <vector>

namespace KetaEngine {

class ISoundRegistry;
class IAudioEncoder;
class IAudioDSPPanel;
class AudioDSPContext;

/// <summary>
/// 波形加工(音声合成・フィルター・ボコーダ)のエディター
/// インターフェースにのみ依存し、具体的な組み立てはAudioDSPEditorFactoryが行う
/// </summary>
class AudioDSPEditor {
public:
    AudioDSPEditor();
    ~AudioDSPEditor();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="registry">サウンドの登録先</param>
    /// <param name="encoder">ファイル書き出しに使うエンコーダー</param>
    void Init(std::unique_ptr<ISoundRegistry> registry, std::unique_ptr<IAudioEncoder> encoder);

    // パネル追加(追加順に表示)
    void AddPanel(std::unique_ptr<IAudioDSPPanel> panel);

    void EditorUpdate();

private:
    // 宣言順 = 構築順(contextはregistry/encoderを参照するので後に置く)
    std::unique_ptr<ISoundRegistry> registry_;
    std::unique_ptr<IAudioEncoder> encoder_;
    std::unique_ptr<AudioDSPContext> context_;
    std::vector<std::unique_ptr<IAudioDSPPanel>> panels_;
};

}; // KetaEngine
