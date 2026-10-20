#include "AudioDSPEditor.h"

using namespace KetaEngine;
#include "Audio/DSP/FFT.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <imgui.h>

namespace {

// enumをComboで選択(EnumにCountが必要)
template <class E, class NameFunc>
bool EnumCombo(const char* label, E& value, NameFunc nameFunc) {
    bool changed = false;
    if (ImGui::BeginCombo(label, nameFunc(value))) {
        for (int i = 0; i < static_cast<int>(E::Count); ++i) {
            const E item = static_cast<E>(i);
            if (ImGui::Selectable(nameFunc(item), item == value)) {
                value   = item;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

bool DragDouble(const char* label, double& value, float speed, double minValue, double maxValue, const char* format = "%.2f") {
    return ImGui::DragScalar(label, ImGuiDataType_Double, &value, speed, &minValue, &maxValue, format);
}

const char* kVoiceFilterTypeNames[] = {
    "LowPass", "BandPass", "HighPass", "Notch", "LowPassOnePole", "HighPassOnePole"};

bool VoiceFilterGui(const char* id, Audio::VoiceFilter& filter) {
    ImGui::PushID(id);
    int type     = static_cast<int>(filter.type);
    bool changed = ImGui::Combo("Type", &type, kVoiceFilterTypeNames, IM_ARRAYSIZE(kVoiceFilterTypeNames));
    filter.type  = static_cast<XAUDIO2_FILTER_TYPE>(type);
    changed |= ImGui::DragFloat("Frequency[Hz]", &filter.frequency, 10.0f, 20.0f, 20000.0f, "%.0f");
    changed |= ImGui::DragFloat("Q", &filter.q, 0.01f, 0.67f, 10.0f, "%.2f");
    ImGui::PopID();
    return changed;
}

} // namespace

void AudioDSPEditor::Init() {
    audio_ = Audio::GetInstance();

    generatorParam_.waveform  = WaveGenerator::Waveform::SineSaw;
    generatorParam_.frequency = 220.0;
    generatorParam_.duration  = 1.5;
}

void AudioDSPEditor::EditorUpdate() {
    if (!audio_) {
        return;
    }
    if (!ImGui::CollapsingHeader("Audio DSP")) {
        return;
    }
    ImGui::PushID("AudioDSPEditor");

    DrawVoiceSynthesizer();
    DrawWaveGenerator();
    DrawOfflineFilter();
    DrawVocoder();
    DrawRealtimeFilter();
    DrawPreview();

    ImGui::PopID();
}

///============================================================
/// 音声合成
///============================================================
void AudioDSPEditor::DrawVoiceSynthesizer() {
    if (!ImGui::TreeNode("音声合成 (Formant)")) {
        return;
    }
    auto& p = synthesizer_.GetParam();

    ImGui::InputText("Text(ローマ字)", synthText_, sizeof(synthText_));
    DragDouble("Pitch[Hz]", p.pitch, 1.0f, 50.0, 800.0, "%.0f");
    DragDouble("1音の長さ[s]", p.moraDuration, 0.01f, 0.05, 1.0);
    DragDouble("フォルマント帯域幅[Hz]", p.formantBandwidth, 1.0f, 10.0, 500.0, "%.0f");
    DragDouble("母音フォルマント倍率", p.vowelFormantScale, 0.01f, 0.5, 2.0);
    DragDouble("子音フォルマント倍率", p.consonantFormantScale, 0.01f, 0.5, 2.0);
    DragDouble("子音の強さ", p.consonantGain, 0.01f, 0.0, 2.0);
    DragDouble("ディエンファシス", p.deEmphasis, 0.001f, 0.0, 0.999, "%.3f");

    if (ImGui::Button("合成")) {
        RegisterResult(voiceSoundName_, synthesizer_.Synthesize(synthText_));
    }
    ImGui::SameLine();
    if (ImGui::Button("再生##voice")) {
        audio_->Play(voiceSoundName_, playVolume_);
    }
    ImGui::TreePop();
}

///============================================================
/// 波形生成
///============================================================
void AudioDSPEditor::DrawWaveGenerator() {
    if (!ImGui::TreeNode("波形生成")) {
        return;
    }
    auto& p = generatorParam_;
    EnumCombo("Waveform", p.waveform, WaveGenerator::GetWaveformName);
    DragDouble("Frequency[Hz]", p.frequency, 1.0f, 20.0, 8000.0, "%.0f");
    DragDouble("Amplitude", p.amplitude, 0.01f, 0.0, 1.0);
    DragDouble("Duration[s]", p.duration, 0.01f, 0.05, 10.0);

    if (ImGui::Button("生成")) {
        RegisterResult(generatorSoundName_, WaveGenerator::Generate(p));
    }
    ImGui::SameLine();
    if (ImGui::Button("再生##wave")) {
        audio_->Play(generatorSoundName_, playVolume_);
    }
    ImGui::TreePop();
}

///============================================================
/// オフラインフィルター
///============================================================
void AudioDSPEditor::DrawOfflineFilter() {
    if (!ImGui::TreeNode("フィルター (FIR / IIR)")) {
        return;
    }
    SoundCombo("Source", filterSource_);

    int kind = static_cast<int>(filterKind_);
    ImGui::RadioButton("IIR", &kind, static_cast<int>(FilterKind::IIR));
    ImGui::SameLine();
    ImGui::RadioButton("FIR", &kind, static_cast<int>(FilterKind::FIR));
    filterKind_ = static_cast<FilterKind>(kind);

    if (filterKind_ == FilterKind::IIR) {
        auto& p = iirParam_;
        EnumCombo("Type", p.type, IIRFilter::GetTypeName);
        const bool isBand = p.type == IIRFilter::Type::BandPass || p.type == IIRFilter::Type::BandEliminate;
        DragDouble(isBand ? "Low[Hz]" : "Frequency[Hz]", p.frequency, 10.0f, 20.0, 20000.0, "%.0f");
        if (isBand) {
            DragDouble("High[Hz]", p.frequency2, 10.0f, 20.0, 20000.0, "%.0f");
        } else {
            DragDouble("Q", p.q, 0.01f, 0.1, 20.0);
        }
    } else {
        auto& p = firParam_;
        EnumCombo("Type", p.type, FIRFilter::GetTypeName);
        const bool isBand = p.type == FIRFilter::Type::BandPass || p.type == FIRFilter::Type::BandEliminate;
        DragDouble(isBand ? "Low[Hz]" : "Frequency[Hz]", p.frequency, 10.0f, 20.0, 20000.0, "%.0f");
        if (isBand) {
            DragDouble("High[Hz]", p.frequency2, 10.0f, 20.0, 20000.0, "%.0f");
        }
        ImGui::DragInt("遅延器の数J", &p.delayCount, 2.0f, 2, 1024);
        EnumCombo("Window", p.window, WindowFunction::GetTypeName);
    }

    if (ImGui::Button("フィルター適用") && !filterSource_.empty()) {
        AudioPCM pcm;
        if (audio_->GetPCM(filterSource_, pcm)) {
            if (filterKind_ == FilterKind::IIR) {
                IIRFilter filter(iirParam_, pcm.GetSampleRate());
                filter.Process(pcm);
            } else {
                FIRFilter filter(firParam_, pcm.GetSampleRate());
                filter.Process(pcm);
            }
            RegisterResult(filterSoundName_, pcm);
        } else {
            statusMessage_ = "未対応のフォーマットです: " + filterSource_;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("再生(元)")) {
        audio_->Play(filterSource_, playVolume_);
    }
    ImGui::SameLine();
    if (ImGui::Button("再生(結果)")) {
        audio_->Play(filterSoundName_, playVolume_);
    }
    ImGui::TreePop();
}

///============================================================
/// ボコーダ
///============================================================
void AudioDSPEditor::DrawVocoder() {
    if (!ImGui::TreeNode("ボコーダ")) {
        return;
    }
    SoundCombo("Carrier(楽器)", vocoderCarrier_);
    SoundCombo("Modulator(声)", vocoderModulator_);

    auto& p = vocoder_.GetParam();
    ImGui::DragInt("FFT Size", &p.fftSize, 1.0f, 64, 8192);
    ImGui::DragInt("Band Width[bin]", &p.bandWidth, 1.0f, 1, 64);
    EnumCombo("Window", p.window, WindowFunction::GetTypeName);

    if (ImGui::Button("ボコーダ適用")) {
        AudioPCM carrier, modulator;
        if (audio_->GetPCM(vocoderCarrier_, carrier) && audio_->GetPCM(vocoderModulator_, modulator)) {
            if (carrier.GetSampleRate() != modulator.GetSampleRate()) {
                statusMessage_ = "サンプリング周波数が異なります";
            } else {
                RegisterResult(vocoderSoundName_, vocoder_.Process(carrier, modulator));
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("再生##vocoder")) {
        audio_->Play(vocoderSoundName_, playVolume_);
    }
    ImGui::TreePop();
}

///============================================================
/// リアルタイムフィルター
///============================================================
void AudioDSPEditor::DrawRealtimeFilter() {
    if (!ImGui::TreeNode("リアルタイムフィルター (XAudio2)")) {
        return;
    }
    ImGui::SliderFloat("Volume", &playVolume_, 0.0f, 1.0f);

    ImGui::SeparatorText("個別 (PlayWithFilter)");
    SoundCombo("Sound", realtimeSource_);
    VoiceFilterGui("voice", voiceFilter_);
    if (ImGui::Button("フィルター付き再生")) {
        audio_->PlayWithFilter(realtimeSource_, voiceFilter_, playVolume_);
    }

    ImGui::SeparatorText("全体 (SetGlobalFilter)");
    bool changed = ImGui::Checkbox("有効", &isGlobalFilterEnabled_);
    changed |= VoiceFilterGui("global", globalFilter_);
    if (changed) {
        if (isGlobalFilterEnabled_) {
            audio_->SetGlobalFilter(globalFilter_);
        } else {
            audio_->ClearGlobalFilter();
        }
    }
    ImGui::TreePop();
}

///============================================================
/// プレビュー
///============================================================
void AudioDSPEditor::DrawPreview() {
    ImGui::SeparatorText("プレビュー");
    if (!statusMessage_.empty()) {
        ImGui::TextWrapped("%s", statusMessage_.c_str());
    }

    std::string selected = previewName_;
    if (SoundCombo("表示するサウンド", selected) && selected != previewName_) {
        SetPreview(selected);
    }
    if (previewWave_.empty()) {
        return;
    }

    ImGui::Text("%.2f秒 / %dHz", previewDuration_, previewSampleRate_);
    ImGui::PlotLines("波形", previewWave_.data(), static_cast<int>(previewWave_.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
    ImGui::PlotLines("スペクトル[dB]", previewSpectrum_.data(), static_cast<int>(previewSpectrum_.size()), 0,
        nullptr, -80.0f, 0.0f, ImVec2(0, 80));
    ImGui::Text("スペクトル横軸: 0 ～ %dHz", previewSampleRate_ / 2);

    if (ImGui::Button("再生##preview")) {
        audio_->Play(previewName_, playVolume_);
    }
    ImGui::SameLine();
    if (ImGui::Button("wav書き出し")) {
        SaveWav(previewName_);
    }
}

///============================================================
/// ヘルパー
///============================================================
bool AudioDSPEditor::SoundCombo(const char* label, std::string& selected) {
    bool changed = false;
    if (ImGui::BeginCombo(label, selected.empty() ? "(none)" : selected.c_str())) {
        std::vector<std::string> names = audio_->GetAvailableSoundNames();
        std::sort(names.begin(), names.end());
        for (const auto& name : names) {
            if (ImGui::Selectable(name.c_str(), name == selected)) {
                selected = name;
                changed  = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

void AudioDSPEditor::RegisterResult(const std::string& soundName, const AudioPCM& pcm) {
    if (audio_->RegisterPCM(soundName, pcm) < 0) {
        statusMessage_ = "生成に失敗しました(長さ0): " + soundName;
        return;
    }
    statusMessage_ = "生成しました: " + soundName;
    SetPreview(soundName);
}

void AudioDSPEditor::SetPreview(const std::string& soundName) {
    previewName_ = soundName;
    previewWave_.clear();
    previewSpectrum_.clear();

    AudioPCM pcm;
    if (!audio_->GetPCM(soundName, pcm) || pcm.IsEmpty()) {
        return;
    }
    previewDuration_   = pcm.GetDuration();
    previewSampleRate_ = pcm.GetSampleRate();

    const std::vector<double> mono = pcm.GetMonoMix();

    // 波形(表示用に間引き)
    const size_t pointCount = (std::min)(mono.size(), size_t(1024));
    previewWave_.resize(pointCount);
    for (size_t i = 0; i < pointCount; ++i) {
        previewWave_[i] = static_cast<float>(mono[i * mono.size() / pointCount]);
    }

    // スペクトル(中央付近を窓掛けしてFFT)
    const size_t fftSize = (std::min)(size_t(4096), FFT::NextPowerOfTwo(mono.size()) / 2);
    if (fftSize < 16) {
        return;
    }
    const size_t start          = (mono.size() - fftSize) / 2;
    const std::vector<double> w = WindowFunction::Generate(WindowFunction::Type::Hanning, static_cast<int>(fftSize));
    std::vector<FFT::Complex> x(fftSize);
    for (size_t i = 0; i < fftSize; ++i) {
        x[i] = FFT::Complex(mono[start + i] * w[i], 0.0);
    }
    FFT::Transform(x, false);

    const std::vector<double> magnitude = FFT::Magnitude(x);
    const double maxMagnitude           = (std::max)(*std::max_element(magnitude.begin(), magnitude.begin() + fftSize / 2), 1.0e-12);
    previewSpectrum_.resize(fftSize / 2);
    for (size_t k = 0; k < fftSize / 2; ++k) {
        const double db     = 20.0 * std::log10((std::max)(magnitude[k] / maxMagnitude, 1.0e-6));
        previewSpectrum_[k] = static_cast<float>((std::max)(db, -80.0));
    }
}

void AudioDSPEditor::SaveWav(const std::string& soundName) {
    AudioPCM pcm;
    if (!audio_->GetPCM(soundName, pcm)) {
        return;
    }
    std::filesystem::create_directories(saveDirectory_);
    const std::string path = saveDirectory_ + std::filesystem::path(soundName).stem().string() + ".wav";
    statusMessage_         = pcm.SaveWav(path) ? "書き出しました: " + path : "書き出しに失敗しました: " + path;
}
