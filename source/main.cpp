#include <windows.h>
#include "Engine/BasicMacros.h"
#include "Engine/IPlatform.h"
#include "Engine/MemoryManager.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// extern the creation of the platform so that we can change it at compile time
extern IPlatform* CreatePlatform();

int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd)
{
	UNUSED_VAR(hInstance);
	UNUSED_VAR(hPrevInstance);

#if defined(_DEBUG) && defined(WIN32)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// set our current working directory to the one that the exe is in
	// so that we can access paths correctly rather than where the exe was launched from
#ifdef WIN32
	wchar_t szPath[MAX_PATH];
	::GetModuleFileNameW(NULL, &szPath[0], MAX_PATH);

	wchar_t* dir_end = wcsrchr(&szPath[0], '\\');
	if (dir_end != NULL)
	{
		*dir_end = '\0';
		::SetCurrentDirectoryW(szPath);
	}
#endif // WIN32

	// before we do anything, create the memory manager
	MemoryManager::CreateMemoryManager();

	IPlatform* platform = CreatePlatform();
	if (platform->Init(nShowCmd, &lpCmdLine))
	{
		while (!platform->IsDone())
		{
			platform->Update();
		}
	}
	
	delete platform;

	MemoryManager::GetMemoryManager()->DebugOutputAllocations();

	return 0;
}