#include"Rail.h"

void Rail::Init() {
	
}
void Rail::Update() {
	objct3D_->Update();
}
void Rail::Draw(const ViewProjection&viewProjection) {
	objct3D_->Draw(viewProjection);
}