#pragma once
#include <string>
#include "../../externals/DirectXTex/DirectXTex.h"

namespace KetaEngine {

class TextureConverter {
public:
    TextureConverter()  = default;
    ~TextureConverter() = default;

    void ConvertTextureWICToDDS(const std::string& filePath);

private:
    void LoadWICTextureFromFile(const std::string& filePath);
    static std::wstring ConvertMultiByteStringToWideString(const std::string& mString);

    void SaveDDSTextureToFile();
    void SeparateFilePath(const std::wstring& filePath);

private:
    DirectX::TexMetadata metadata_;
    DirectX::ScratchImage scratchImage_;

    std::wstring directoryPath_;
    std::wstring fileName_;
    std::wstring fileExt_;
};
}