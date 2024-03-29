﻿#include <Windows.h>
#include <stdexcept>

#include <CMVS/VFS_Hook.h>
#include <Rut/RxPath.h>
#include <Rut/RxINI.h>
#include <RxHook/API.h>

using namespace Rut;


static void HookFont(RxINI::Parser& rfINN)
{
	auto save_str_on_heap = [](const std::string& msStr) -> char*
		{
			char* pStr = new char[msStr.size() + 1];
			memcpy(pStr, msStr.c_str(), msStr.size() + 1);
			return pStr;
		};

	bool is_hook_font_a = rfINN[L"HookFont"][L"HookCreateFontA"];
	bool is_hook_font_indirect_a = rfINN[L"HookFont"][L"HookCreateFontIndirectA"];

	if (is_hook_font_a)
	{
		RxHook::HookCreateFontA(rfINN[L"HookFont"][L"Charset"], save_str_on_heap(rfINN[L"HookFont"][L"FontName"]));
	}

	if (is_hook_font_indirect_a)
	{
		RxHook::HookCreateFontIndirectA(rfINN[L"HookFont"][L"Charset"], save_str_on_heap(rfINN[L"HookFont"][L"FontName"]));
	}
}

static void HookFile(RxINI::Parser& rfINN)
{
	std::wstring seleted_game_name = rfINN.Get(L"CMVS_File_Hook", L"GameSelected");

	auto& seleted_game = rfINN.Get(seleted_game_name);
	std::wstring version = seleted_game[L"Engine_Ver"];
	uint32_t script_loa = seleted_game[L"Script_Lod"];
	uint32_t script_xor = seleted_game[L"Script_Xor"];
	uint32_t script_com = seleted_game[L"Script_Com"];
	uint32_t image_load = seleted_game[L"Image_Load"];

	std::string hook_folder = rfINN.Get(L"CMVS_File_Hook", L"HookFolder");
	CMVS::VFS::SetHookFolder(hook_folder.c_str());

	if (version == L"CMVS_380_")
	{
		//CMVS::FileHook::SetPS3Hook_380_(script_loa);
		//CMVS::FileHook::SetPB3Hook_380_(image_load);
	}
	else if (version == L"CMVS_342_")
	{
		//CMVS::FileHook::SetPS3Hook_342_(script_loa, script_xor, script_com);
		//CMVS::FileHook::SetPB3Hook_342_(image_load);
	}
	else if (version == L"CMVS_210_")
	{
		//CMVS::FileHook::SetPS3Hook_210_(script_loa, script_xor, script_com);
		//CMVS::FileHook::SetPB3Hook_210_(image_load);
	}
}

static void StartHook(HMODULE hDllBase)
{
	try
	{
		RxINI::Parser ini(RxPath::RemoveSuffix(RxPath::ModulePathW(hDllBase)) + L".ini");
		HookFont(ini);
		HookFile(ini);
	}
	catch (const std::runtime_error& err)
	{
		MessageBoxA(NULL, err.what(), NULL, NULL);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		StartHook(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" VOID __declspec(dllexport) Dir_A() {}