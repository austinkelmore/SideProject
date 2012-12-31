
#include "Logging.h"
#include <stdio.h>
#include <nowide/convert.h>
#include "JSONConfig.h"
#include "BasicMacros.h"

#ifdef WIN32
	#include <windows.h>
#endif // WIN32

IMPLEMENT_CONFIG(Log, Logging)
{
	ADD_PROPS(int, Flush_Frequency, 15);
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

void Logging::Log(LogChannel channel, const char *format, ...)
{
	UNUSED_VAR(channel);

	// see if we want to early out
	if (g_log && !g_log->IsChannelEnabled(channel))
		return;

	const unsigned int MAX_CHARS = 1023;
	static char buffer[MAX_CHARS + 1];

	va_list argList;
	va_start( argList, format );

	int chars_written = vsprintf_s(buffer, MAX_CHARS, format, argList);
	buffer[chars_written] = '\0';

#ifdef WIN32
	OutputDebugStringW(nowide::convert(buffer).c_str());
#endif // WIN32

	if (g_log)
	{
		if (g_log->GetFlushCount() == 0)
		{
			g_log->SetFlushCount(g_log->GetProps()->Flush_Frequency);
			fflush(stdout);
		}
		else
			g_log->SetFlushCount(g_log->GetFlushCount() - 1);
	}
}

void Logging::EnableChannel(const LogChannel logChannel)
{
	// make sure we don't have it in the list already
	for (unsigned int i = 0; i < m_disabledChannels.size(); ++i)
	{
		DBG_ASSERT(m_disabledChannels[i] != logChannel);
	}

	m_disabledChannels.push_back(logChannel);
}

void Logging::DisableChannel(const LogChannel logChannel)
{
	for (unsigned int i = 0; i < m_disabledChannels.size(); ++i)
	{
		if (m_disabledChannels[i] == logChannel)
		{
			m_disabledChannels.erase(m_disabledChannels.begin()+i);
			return;
		}
	}
}

bool Logging::IsChannelEnabled(const LogChannel logChannel) const
{
	for (unsigned int i = 0; i < m_disabledChannels.size(); ++i)
	{
		if (m_disabledChannels[i] == logChannel)
			return false;
	}

	return true;
}
