#include <Windows.h>
#include <fstream>
#include "Install.hpp"
#include "BASS/bass.h"
#include "SDK.hpp"
#include "helpers/Utils.hpp"
#include <string>


BOOL __stdcall DllMain(HINSTANCE mod, DWORD dwReason, LPVOID res)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:

		{
			Global::hmDll = mod;
			BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));
			CreateThread(0, 0, &Installer::LoadHanaLovesMe, 0, 0, 0);
		}

		break;
	case DLL_PROCESS_DETACH:
		Installer::UnLoadHanaLovesMe();
		break;
	}
	return TRUE;
}

bool FileExists(std::string &file)
{
	std::ifstream in(file);
	return in.good();
}
