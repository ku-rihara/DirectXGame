#pragma once
#include<string>
#include<format>

static void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}
