#include <windows.h>
#include "Engine/Framework/Win32Framework.h"

#include "jsoncpp/json/json.h"
#include <fstream>
#include <iostream>
#include <algorithm>

#include <nowide/convert.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// extern the creation of these systems so they work with any systems that are compiled in
extern IFramework* CreateFramework();

using std::ifstream;

void __cdecl odprintf( const char *str, ... )
{
	char buf[2048];

	va_list ptr;
	va_start( ptr, str );
	vsprintf_s( buf, str, ptr );

	OutputDebugStringW( nowide::convert( buf ).c_str() );
}

static void printValueTree( Json::Value &value, const std::string &path = "." )
{
	switch ( value.type() )
	{
	case Json::nullValue:
		odprintf( "%s=null\n", path.c_str() );
		break;
	case Json::intValue:
		odprintf( "%s=%s\n", path.c_str(), Json::valueToString( value.asLargestInt() ).c_str() );
		break;
	case Json::uintValue:
		odprintf( "%s=%s\n", path.c_str(), Json::valueToString( value.asLargestUInt() ).c_str() );
		break;
	case Json::realValue:
		odprintf( "%s=%.16g\n", path.c_str(), value.asDouble() );
		break;
	case Json::stringValue:
		odprintf( "%s=\"%s\"\n", path.c_str(), value.asString().c_str() );
		break;
	case Json::booleanValue:
		odprintf( "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false" );
		break;
	case Json::arrayValue:
		{
			odprintf( "%s=[]\n", path.c_str() );
			int size = value.size();
			for ( int index =0; index < size; ++index )
			{
				static char buffer[16];
				sprintf( buffer, "[%d]", index );
				printValueTree( value[index], path + buffer );
			}
		}
		break;
	case Json::objectValue:
		{
			odprintf( "%s={}\n", path.c_str() );
			Json::Value::Members members( value.getMemberNames() );
			std::sort( members.begin(), members.end() );
			std::string suffix = *(path.end()-1) == '.' ? "" : ".";
			for ( Json::Value::Members::iterator it = members.begin(); 
				it != members.end(); 
				++it )
			{
				const std::string &name = *it;
				printValueTree( value[name], path + suffix + name );
			}
		}
		break;
	default:
		break;
	}
}

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

	// do something
	Json::Reader reader;
	Json::Value rootValue;

	std::ifstream configFile( "test.json", ifstream::in );
	bool successfull = reader.parse( configFile, rootValue );

	if ( !successfull )
	{
		std::cout << "Failed to parse configuration\n" << reader.getFormattedErrorMessages();
	}

	printValueTree( rootValue );

	while ( !framework->IsDone() )
	{
		framework->Update();
	}

	delete framework;

	return 0;
}