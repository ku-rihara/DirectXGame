#include "Base/WinApp.h"
#include "Core/KTGame.h"
#include "Engine/utility/Log/Log.h"

// windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    KetaEngine::Log::Info("Engine Initialize");

    std::unique_ptr<KetaEngine::KTFramework> game = std::make_unique<KetaEngine::KTGame>();
    game->Run();

    return 0;
}
