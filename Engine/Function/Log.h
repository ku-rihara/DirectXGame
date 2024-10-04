#pragma once
#include<string>
#include<format>
#include <iostream>

static void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}
