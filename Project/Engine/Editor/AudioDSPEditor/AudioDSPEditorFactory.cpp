#include "AudioDSPEditorFactory.h"

using namespace KetaEngine;
#include "Audio/DSP/IO/AudioSoundRegistry.h"
#include "Audio/DSP/IO/MP3Encoder.h"
#include "AudioDSPEditor.h"
#include "Panel/FilterPanel.h"
#include "Panel/OscillatorPanel.h"
#include "Panel/PreviewPanel.h"
#include "Panel/RealtimeFilterPanel.h"
#include "Panel/VocoderPanel.h"
#include "Panel/VoiceSynthPanel.h"

std::unique_ptr<AudioDSPEditor> AudioDSPEditorFactory::Create(Audio& audio) const {
    auto editor = std::make_unique<AudioDSPEditor>();
    editor->Init(std::make_unique<AudioSoundRegistry>(audio), std::make_unique<MP3Encoder>());

    editor->AddPanel(std::make_unique<VoiceSynthPanel>());
    editor->AddPanel(std::make_unique<OscillatorPanel>());
    editor->AddPanel(std::make_unique<FilterPanel>());
    editor->AddPanel(std::make_unique<VocoderPanel>());
    editor->AddPanel(std::make_unique<RealtimeFilterPanel>(audio));
    editor->AddPanel(std::make_unique<PreviewPanel>());
    return editor;
}
