
#pragma once

#include "JSONConfig.h"

enum LogChannel
{
	LOG_UI = 0,
	LOG_Gameplay,
	LOG_Graphics,
	LOG_Physics,
	LOG_Input,
	LOG_FileIO,
	LOG_Network,
	LOG_Platform,
	LOG_Config
};

enum LogTarget
{
	LOGTARGET_STDOUT,
	LOGTARGET_FILE
};

class Logging
{
public:
	DEFINE_CONFIG(Log)
		int Flush_Frequency;
	DEFINE_CONFIG_END()

	Logging();
	~Logging();

	static void Log(LogChannel channel, const char *format, ...);

	int GetFlushCount() { return _current_flush_count; }
	void SetFlushCount(const int flush_count) { _current_flush_count = flush_count; }

private:
	int _current_flush_count; // how long ago we last flushed
};

extern Logging* g_log;
