#include <windows.h>
#include "Engine/Framework/Win32Framework.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

extern IFramework* CreateFramework();

int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{

#if defined(_DEBUG) && defined(WIN32)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	IFramework* framework = CreateFramework();
	if ( !framework->Init() )
		return -1;

	// do something

	while ( true )
	{
		framework->Update();
	}

	delete framework;

	return 0;
}