#include "base/WinApp.h"
#include "Core/KTGame.h"

// windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    std::unique_ptr<KTFramework> game = std::make_unique<KTGame>();
    game->Run();

    return 0;
}
