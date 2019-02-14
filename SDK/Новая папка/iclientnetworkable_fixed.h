#pragma once
#include "virtuals.h"
#include "../Stools/SDK_base.h"

class IClientNetworkable_fixed {
public:
	ClientClass* GetClientClass(void) {
		return GetVirtualFunction<ClientClass*(__thiscall *)(IClientNetworkable_fixed*)>(this, 1)(this);
	 }
	bool IsDormant(void) {
		return GetVirtualFunction<bool(__thiscall *)(IClientNetworkable_fixed*)>(this, 7)(this);
	}
};

ClientClass *GetEntityClientClass(IClientEntity *pEnt) {
	if (pEnt) {
		IClientNetworkable_fixed *Icl_net_fix = (IClientNetworkable_fixed*)pEnt->GetClientNetworkable();
		ClientClass *cl_class = Icl_net_fix->GetClientClass();
		return cl_class;
	}
	return NULL;
}

bool IsDormant(IClientEntity *pEnt) {
	if (pEnt) {
		IClientNetworkable_fixed *Icl_net_fix = (IClientNetworkable_fixed*)pEnt->GetClientNetworkable();
		bool is_dormant = Icl_net_fix->IsDormant();
		return is_dormant;
	}
}