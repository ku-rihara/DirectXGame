#include "Core/ResourceLoader.h"
#include "Base/TextureManager.h"
#include "3d/ModelManager.h"
#include "Audio/Audio.h"

using namespace KetaEngine;

void ResourceLoader::LoadAllResources() {
    // テクスチャ全ロード
    TextureManager::GetInstance()->LoadAllTextures();
    // モデル全ロード
    ModelManager::LoadAllModels();
    // オーディオ全ロード
    Audio::GetInstance()->LoadAllFilesInDirectory();
}
