#include "Base/WinApp.h"
#include "Core/KTGame.h"

// windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::unique_ptr<KetaEngine::KTFramework> game = std::make_unique<KetaEngine::KTGame>();
    game->Run();

    return 0;
}
