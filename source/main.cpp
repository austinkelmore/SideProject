#include <windows.h>
#include "Engine/Framework/Win32Framework.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Config/JSONConfig.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// extern the creation of these systems so they work with any systems that are compiled in
extern IFramework* CreateFramework();


int WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{
#if defined(_DEBUG) && defined(WIN32)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#ifdef WIN32
	// set our current working directory to the one that the exe is in
	wchar_t szPath[MAX_PATH];
	::GetModuleFileNameW( NULL, &szPath[0], MAX_PATH );

	wchar_t* dir_end = wcsrchr( &szPath[0], '\\' );
	if ( dir_end != 0x0 )
	{
		*dir_end = '\0';
		::SetCurrentDirectoryW( szPath );
	}
#endif // WIN32

	IFramework* framework = CreateFramework();
	if ( !framework->Init() )
		return -1;

	// set up the logging
	gLog = new Logging();

	// gConfig is guaranteed to be initialized because of the static props initializing it
	gConfig->ReadConfig("test.json");
	gConfig->Initialize();
	gConfig->DebugPrintValueStream();

	while ( !framework->IsDone() )
	{
		framework->Update();
	}

	delete framework;
	delete gLog;
	delete gConfig;

	return 0;
}