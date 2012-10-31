
#include "Logging.h"
#include <stdio.h>
#include <nowide/convert.h>
#include "JSONConfig.h"

#ifdef WIN32
	#include <windows.h>
#endif // WIN32

IMPLEMENT_CONFIG(Log, Logging)
{
	ADD_PROPS(int, Flush_Frequency);
}

Logging* g_log = NULL;

Logging::Logging()
{
	// do nothing
	_current_flush_count = 0;
}

Logging::~Logging()
{
	// do nothing
	g_log = NULL;
}

void Logging::Log( LogChannel channel, const char *format, ... )
{
	const unsigned int MAX_CHARS = 1023;
	static char buffer[MAX_CHARS + 1];

	va_list argList;
	va_start( argList, format );

	int chars_written = vsprintf_s( buffer, MAX_CHARS, format, argList );
	buffer[chars_written] = '\0';

#ifdef WIN32
	OutputDebugStringW( nowide::convert(buffer).c_str() );
#endif // WIN32

	if ( _current_flush_count == 0 )
	{
		_current_flush_count = GetProps()->Flush_Frequency;
		fflush(stdout);
	}
	else
		--_current_flush_count;
}
