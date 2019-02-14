#pragma once
#include "virtuals.h"

class IVEngineClient_fixed {
public:
	void ClientCmd_Unrestricted(const char* command) {
		return GetVirtualFunction<void(__thiscall *)(IVEngineClient_fixed*, const char*)>(this, 107)(this, command);
	}
};