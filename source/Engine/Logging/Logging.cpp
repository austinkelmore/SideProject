
#include "Logging.h"
#include <stdio.h>
#include <nowide/convert.h>

#ifdef WIN32
#  include <windows.h>
#endif // WIN32

Logging* gLog = NULL;

Logging::Logging()
{
	// do nothing
	mFlushFrequency = 0;
	mCurrentFlushCount = 0;
}

Logging::~Logging()
{
	// do nothing
	gLog = NULL;
}

void Logging::Log( LogChannel channel, const char *format, ... )
{
	const unsigned int MAX_CHARS = 1023;
	static char buffer[MAX_CHARS + 1];

	va_list argList;
	va_start( argList, format );

	int charsWritten = vsprintf_s( buffer, MAX_CHARS, format, argList );
	buffer[MAX_CHARS] = '\0';

#ifdef WIN32
	OutputDebugStringW( nowide::convert(buffer).c_str() );
#endif // WIN32

	if ( mCurrentFlushCount == 0 )
	{
		mCurrentFlushCount = mFlushFrequency;
		fflush(stdout);
	}
	else
		--mCurrentFlushCount;
}
