#include<Windows.h>

//windowアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	OutputDebugStringA("Hello,DirectX!\n");

	return 0;
}