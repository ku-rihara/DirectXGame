#include "PreviewPanel.h"

using namespace KetaEngine;
#include "Audio/DSP/Core/AudioPCM.h"
#include "Audio/DSP/IO/IAudioEncoder.h"
#include "Audio/DSP/IO/ISoundRegistry.h"
#include "Editor/AudioDSPEditor/AudioDSPContext.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

void PreviewPanel::Draw(AudioDSPContext& context) {
    // 選択変更
    std::string selected = context.GetPreviewName();
    if (context.GetGui().SoundCombo("表示するサウンド", selected, context.GetRegistry())) {
        context.SelectPreview(selected);
    }

    // 生成・選択されたら作り直す
    if (builtRevision_ != context.GetPreviewRevision()) {
        builtRevision_ = context.GetPreviewRevision();
        Rebuild(context);
    }
    if (wave_.empty()) {
        return;
    }

    ImGui::Text("%.2f秒 / %dHz", duration_, sampleRate_);
    ImGui::PlotLines("波形", wave_.data(), static_cast<int>(wave_.size()), 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
    if (!spectrum_.empty()) {
        ImGui::PlotLines("スペクトル[dB]", spectrum_.data(), static_cast<int>(spectrum_.size()), 0, nullptr, -80.0f, 0.0f, ImVec2(0, 80));
        ImGui::Text("スペクトル横軸: 0 ～ %dHz", sampleRate_ / 2);
    }

    if (ImGui::Button("再生")) {
        context.Play(context.GetPreviewName());
    }

    ImGui::SeparatorText("書き出し");
    ImGui::InputText("ファイル名", exportName_, sizeof(exportName_));
    ImGui::SameLine();
    ImGui::TextUnformatted(context.GetEncoder().GetExtension());
    if (ImGui::Button("書き出し")) {
        Export(context);
    }
}

void PreviewPanel::Rebuild(AudioDSPContext& context) {
    wave_.clear();
    spectrum_.clear();

    AudioPCM pcm;
    if (!context.GetRegistry().TryGet(context.GetPreviewName(), pcm) || pcm.IsEmpty()) {
        return;
    }
    duration_   = pcm.GetDuration();
    sampleRate_ = pcm.GetSampleRate();

    // 書き出し名の初期値はサウンド名(拡張子なし)
    const std::string stem = std::filesystem::path(context.GetPreviewName()).stem().string();
    strncpy_s(exportName_, stem.c_str(), _TRUNCATE);

    const std::vector<double> mono = pcm.GetMonoMix();
    BuildWave(mono);
    BuildSpectrum(mono);
}

void PreviewPanel::BuildWave(const std::vector<double>& mono) {
    // 表示用に間引き
    const size_t count = (std::min)(mono.size(), wavePointCount_);
    wave_.resize(count);
    for (size_t i = 0; i < count; ++i) {
        wave_[i] = static_cast<float>(mono[i * mono.size() / count]);
    }
}

void PreviewPanel::BuildSpectrum(const std::vector<double>& mono) {
    // 波形長に収まる2のべき乗サイズで中央付近をFFT
    size_t size = maxFFTSize_;
    while (size > mono.size() && size > 16) {
        size >>= 1;
    }
    if (size > mono.size()) {
        return;
    }
    fft_.Resize(size);

    const size_t start          = (mono.size() - size) / 2;
    const std::vector<double> w = window_.Generate(static_cast<int>(size));
    std::vector<FFT::Complex> x(size);
    for (size_t i = 0; i < size; ++i) {
        x[i] = FFT::Complex(mono[start + i] * w[i], 0.0);
    }
    fft_.Forward(x);

    const std::vector<double> magnitude = fft_.Magnitude(x);
    const size_t half                   = size / 2;
    const double maxMagnitude           = (std::max)(*std::max_element(magnitude.begin(), magnitude.begin() + half), 1.0e-12);

    spectrum_.resize(half);
    for (size_t k = 0; k < half; ++k) {
        const double db = 20.0 * std::log10((std::max)(magnitude[k] / maxMagnitude, 1.0e-6));
        spectrum_[k]    = static_cast<float>((std::max)(db, -80.0));
    }
}

void PreviewPanel::Export(AudioDSPContext& context) {
    AudioPCM pcm;
    if (!context.GetRegistry().TryGet(context.GetPreviewName(), pcm)) {
        context.SetStatus("書き出すサウンドがありません");
        return;
    }
    if (exportName_[0] == '\0') {
        context.SetStatus("ファイル名を入力してください");
        return;
    }

    IAudioEncoder& encoder     = context.GetEncoder();
    const std::string fileName = std::string(exportName_) + encoder.GetExtension();
    const std::string path     = exportDirectory_ + fileName;

    if (encoder.Encode(pcm, path)) {
        context.SetStatus("書き出しました: " + path + "  (Play(\"Voice/" + fileName + "\") で再生可)");
    } else {
        context.SetStatus("書き出し失敗: " + encoder.GetLastErrorMessage());
    }
}
