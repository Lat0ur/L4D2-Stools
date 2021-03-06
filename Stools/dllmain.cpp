#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <string>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <d3d9.h>

//#include <memory>
//#include <array>
//#include <fstream>
//#include <map>
//#include <algorithm>
//#include <chrono>
//#include <cstdint>
//#include <stdint.h>
//#include <math.h>

//#include "SDK_base.h"

#include "vmthook.h"
#include "findPattern.h"
#include "Renderer.h"
#include "stools_menu.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"

/*#include "interface.h"
#include "icliententitylist.h"
#include "icliententity.h"
#include "client_class.h"
#include "ivrenderview.h"
#include "engine/ivdebugoverlay.h"
#include "cdll_int.h"
#include "convar.h"

//Fixed interfaces
#include "l4d2_con_msg.h"
#include "iclientnetworkable_fixed.h"*/

#include "Interface.h"
#include "ivrenderview_fix.h"
#include "cdll_int_fix.h"

#include "L4D2_SDK.h"

//-------------------------D3D----------------------------
IDirect3DDevice9* d3d9_device = NULL;
std::unique_ptr<VMTHook> d3d9_hook;
typedef HRESULT(__stdcall *EndScene_t) (IDirect3DDevice9*);
typedef HRESULT(__stdcall *Reset_t) (IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

//----------------------RenderView-------------------------
CVMTHookManager* pEngineStats_BeginFrame;
typedef void(__stdcall *EngineStats_BeginFrame_)(void);
EngineStats_BeginFrame_ oEngineStats_BeginFrame;

//Interfaces
IClientEntityList *entList = NULL;
IVRenderView  *renderView = NULL;
IVDebugOverlay *debug = NULL;
IBaseClientDLL *clientDLL = NULL;
IVEngineClient	*engineClient = NULL;

Renderer renderer;

void Test() {

	/*IClientEntity *ent = NULL;
	ClientClass *cls = NULL;
	for (int i = 0; i < entList->GetMaxEntities(); i++) {
		ent = entList->GetClientEntity(i);
		if (ent) {
			cls = GetEntityClientClass(ent);
			//L4D2_ConColorMsg(Color(0, 255, 0, 255), "%s\n", cls->GetName());
			debug->AddEntityTextOverlay(i, 0, 0, 255, 255, 255, 255, "%s", cls->GetName());
		}
	}*/
}

void DrawMenu() {
	/*const char *entity_names[1024];
	int counter = 0;

	IClientEntity *ent = NULL;
	ClientClass *cls = NULL;
	for (int i = 0; i < entList->GetMaxEntities(); i++) {
		ent = entList->GetClientEntity(i);
		if (ent) {
			cls = GetEntityClientClass(ent);
			L4D2_ConColorMsg(Color(0, 255, 0, 255), "%s\n", cls->GetName());
			entity_names[counter] = cls->GetName();
			counter++;
		}
	}

	ImGui::Begin("Window");
	//const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	static int listbox_item_current = 1;
	ImGui::Text("Entitys:");
	ImGui::ListBox("", &listbox_item_current, entity_names, counter, 15);
	*/
	ImGui::Begin("Senny");
	ImGui::Text("Hello world!");
	ImGui::ShowDemoWindow();
	ImGui::End();
}

HRESULT __stdcall hkEndScene(IDirect3DDevice9* pDevice) {
	static EndScene_t oEndScene = d3d9_hook->GetOriginalFunction<EndScene_t>(42);
	//D3DRECT Brect = { 100,100,200,200 };
	//pDevice->Clear(1, &Brect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(1, 1, 1, 1), 0.0f, 0);

	// Determines whether the mouse is enabled in-game.
	static bool mouse_enabled = false;

	// Check whether the GUI is currently open.
	bool is_renderer_active = renderer.IsActive();

	if (is_renderer_active) {
		if (mouse_enabled) {
			// Disable the mouse while the menu is open.
			//engine->ClientCmd_Unrestricted("cl_mouseenable 0");
			//engineClient_fixed->ClientCmd_Unrestricted("debugsystemui ");
			//cancelselect
			//iPanel->SetMouseInputEnabled(1, true);
			mouse_enabled = false;
		}
	}
	else {
		if (!mouse_enabled) {
			// Re-enable the mouse while the menu is closed.
			//engineClient->ClientCmd_Unrestricted("cl_mouseenable 1");
			//engineClient_fixed->ClientCmd_Unrestricted("debugsystemui ");
			//iPanel->SetMouseInputEnabled(1, false);
			mouse_enabled = true;
		}
	}

	// Enable the in-built cursor while the GUI is active.
	ImGui::GetIO().MouseDrawCursor = is_renderer_active;

	// Don't do anything else if the GUI is inactive.
	if (!is_renderer_active)
		return oEndScene(pDevice);

	// Let ImGui know we've started a new frame.
	ImGui_ImplDX9_NewFrame();

	// Queue up draw calls for the interface.
	DrawMenu();

	// Render everything in the draw lists.
	ImGui::Render();
	
	return oEndScene(pDevice);
}

HRESULT __stdcall hkReset(IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* params) {
	// Get the original function and store it in a static variable for later usage.
	static Reset_t oReset = d3d9_hook->GetOriginalFunction<Reset_t>(16);

	// Nothing special to do until our renderer is ready.
	if (!renderer.IsReady())
		return oReset(thisptr, params);

	// Destroy any ImGui related resources..
	ImGui_ImplDX9_InvalidateDeviceObjects();

	// ..call the original 'Reset' function..
	HRESULT result = oReset(thisptr, params);

	// ..then recreate them.
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}

VOID WINAPI EngineStats_BeginFrame_Hooked(void)
{
	oEngineStats_BeginFrame();
}

void InitInterfaces(){
	//CreateInterfaceFn engineFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
	//CreateInterfaceFn clientFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface");

	//printf("Factorys:\n");
	//printf("EngineFactory: %08x\n", engineFactory);
	//printf("ClientFactory: %08x\n\n", clientFactory);

	entList = CaptureInterface<IClientEntityList>("client.dll", "VClientEntityList003");
	renderView = CaptureInterface<IVRenderView>("engine.dll", "VEngineRenderView013");
	debug = CaptureInterface<IVDebugOverlay>("engine.dll", "VDebugOverlay003");
	clientDLL = CaptureInterface<IBaseClientDLL>("client.dll", "VClient016");
	engineClient = CaptureInterface<IVEngineClient>("engine.dll", "VEngineClient013");

	printf("Interfaces:\n");
	printf("IClientEntityList: %08x\n", entList);
	printf("IVRenderView: %08x\n", renderView);
	printf("IVDebugOverlay: %08x\n", debug);
	printf("IVEngineClient: %08x\n", engineClient);
	printf("IBaseClientDLL: %08x\n", clientDLL);
}

void InitHooks(){
	d3d9_device = **reinterpret_cast<IDirect3DDevice9***>(FindPattern("shaderapidx9.dll", "\xA1\x00\x00\x00\x00\x8B\x08\x8B\x51\x0C\x50\xFF\xD2\x8B\xF8", "x????xxxxxxxxxx") + 1);
	d3d9_hook = std::make_unique<VMTHook>(d3d9_device);
	d3d9_hook->HookFunction(hkReset, 16);
	d3d9_hook->HookFunction(hkEndScene, 42);

	pEngineStats_BeginFrame = new CVMTHookManager((PDWORD*)renderView);
	oEngineStats_BeginFrame = (EngineStats_BeginFrame_)pEngineStats_BeginFrame->dwHookMethod((DWORD)EngineStats_BeginFrame_Hooked, 8);	
}

void InitRender() {
	renderer.Initialize(FindWindowA("Valve001", NULL), d3d9_device);
}

void WINAPI StoolsInit() {
	InitInterfaces();
	InitHooks();
	InitRender();
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if(ul_reason_for_call==DLL_PROCESS_ATTACH){
		AllocConsole();
		freopen("conin$", "r", stdin);
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
		CreateThread(0, 0, LPTHREAD_START_ROUTINE(StoolsInit), 0, 0, 0);
		printf("Stools loaded!\n\n");
	}
    return true;
}

