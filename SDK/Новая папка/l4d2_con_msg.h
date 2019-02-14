#pragma once
#include "windows.h"

void L4D2_Msg(const char* message) {
	typedef void(*addr)(const char* pMsg, ...);
	addr msg;
	msg = (addr)GetProcAddress(GetModuleHandleA("tier0.dll"), "Msg");
	msg(message);
}
void L4D2_Warning(const char* message) {
	typedef void(*addr)(const char* pMsg, ...);
	addr warning;
	warning = (addr)GetProcAddress(GetModuleHandleA("tier0.dll"), "Warning");
	warning(message);
}

void L4D2_ConColorMsg(const Color& color, const char* message, ...) {
	typedef void(*addr)(const Color& clr, const tchar* pMsg, ...);
	addr concolor;
	concolor = (addr)GetProcAddress(GetModuleHandleA("tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ");
	concolor(color, message);
}