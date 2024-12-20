#include"InputHandle.h"
#include"input/Input.h"

void InputHandler::AssignMoveLeftCommand2PressKeyA() {
	ICommand* command = new MoveLeftCommand();
	this->pressKeyA_ = command;
}

void InputHandler::AssignMoveRightCommand2PressKeyD() {
	ICommand* command = new MoveRightCommand();
	this->pressKeyD_ = command;
}

ICommand* InputHandler::HandleInput() {
	if (Input::GetInstance()->PushKey(DIK_D)) {
		return pressKeyD_;
	}
	else if (Input::GetInstance()->PushKey(DIK_A)) {
		return pressKeyA_;
	}

	return nullptr;
}