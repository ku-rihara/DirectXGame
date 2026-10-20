#pragma once

#include <string>

namespace KetaEngine {

class AudioPCM;

/// <summary>
/// 波形データをファイルへ書き出すエンコーダーのインターフェース
/// </summary>
class IAudioEncoder {
public:
    virtual ~IAudioEncoder() = default;

    /// <summary>
    /// 書き出し
    /// </summary>
    /// <param name="pcm">波形データ</param>
    /// <param name="filePath">出力先(拡張子込み、フォルダが無ければ作成する)</param>
    /// <returns>成功したか(失敗理由はGetLastErrorMessage)</returns>
    virtual bool Encode(const AudioPCM& pcm, const std::string& filePath) = 0;

    // このエンコーダーが出力する拡張子(".mp3"等)
    virtual const char* GetExtension() const = 0;

    virtual const std::string& GetLastErrorMessage() const = 0;
};

}; // KetaEngine
