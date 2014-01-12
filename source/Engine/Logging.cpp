
#include "Logging.h"
#include <stdio.h>
#include <nowide/convert.h>
#include "BasicMacros.h"

#ifdef WIN32
	#include <windows.h>
#endif // WIN32

IMPLEMENT_CONFIG(LogConfig)
{
	ADD_CONFIG_VAR(int, Flush_Frequency, 15);
	ADD_CONFIG_VAR(bool, Prepend_Group_Name, true);
}

Logging* g_log = NULL;

Logging::Logging()
{
	// do nothing
	_current_flush_count = 0;
	_log_config = LogConfig::StaticNew();
}

Logging::~Logging()
{
	// do nothing
	delete _log_config;
	g_log = NULL;
}

void Logging::Log(LogChannel channel, const char *format, ...)
{
	// see if we want to early out
	if (g_log && !g_log->IsChannelEnabled(channel))
		return;

	const unsigned int MAX_CHARS = 1023;
	static char buffer[MAX_CHARS + 1];
	int groupStringLength = 0;

	va_list argList;
	va_start( argList, format );

	// we have to check to see if g_log is valid because it may not be yet (if the config system is logging)
	if (g_log)
	{
		if (g_log->_log_config->Prepend_Group_Name)
		{
			groupStringLength = sprintf_s(buffer, MAX_CHARS, "[%s] ", g_log->GetChannelString(channel));
		}
	}

	int chars_written = vsprintf_s(buffer + groupStringLength, MAX_CHARS - groupStringLength, format, argList);
	buffer[chars_written + groupStringLength] = '\n';
	buffer[chars_written + groupStringLength + 1] = '\0';

#ifdef WIN32
	OutputDebugStringW(nowide::convert(buffer).c_str());
#endif // WIN32

	if (g_log)
	{
		if (g_log->GetFlushCount() == 0)
		{
			g_log->SetFlushCount(g_log->_log_config->Flush_Frequency);
			fflush(stdout);
		}
		else
			g_log->SetFlushCount(g_log->GetFlushCount() - 1);
	}
}

void Logging::EnableChannel(const LogChannel logChannel)
{
	// make sure we don't have it in the list already
	for (unsigned int i = 0; i < _disabledChannels.size(); ++i)
	{
		DBG_ASSERT(_disabledChannels[i] != logChannel);
	}

	_disabledChannels.push_back(logChannel);
}

void Logging::DisableChannel(const LogChannel logChannel)
{
	for (unsigned int i = 0; i < _disabledChannels.size(); ++i)
	{
		if (_disabledChannels[i] == logChannel)
		{
			_disabledChannels.erase(_disabledChannels.begin()+i);
			return;
		}
	}
}

bool Logging::IsChannelEnabled(const LogChannel logChannel) const
{
	for (unsigned int i = 0; i < _disabledChannels.size(); ++i)
	{
		if (_disabledChannels[i] == logChannel)
			return false;
	}

	return true;
}

const char* Logging::GetChannelString(const LogChannel logChannel) const
{
	char* channelStrings[] = { "Assert", "Config", "FileIO", "Gameplay",
							   "Graphics", "Input", "Memory", "Network",
							   "Physics", "Platform", "UI" };

	DBG_ASSERT(0 <= logChannel && logChannel < sizeof(channelStrings)/sizeof(channelStrings[0]));
	return channelStrings[logChannel];
}
