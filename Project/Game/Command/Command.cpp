#include"Command.h"
#include"Player/Player.h"

// ICommandのデストラクタを定義
ICommand::~ICommand() {}

void MoveRightCommand::Exec(Player& player) {
	player.MoveRight();
}

void MoveLeftCommand::Exec(Player& player) {
	player.MoveLeft();
}