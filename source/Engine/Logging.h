
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
	LOG_Engine,
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
	{
		Flush_Frequency = 15;
	}
	int Flush_Frequency;
	DEFINE_CONFIG_END(Log)

	Logging();
	~Logging();

	void Log( LogChannel channel, const char *format, ... );

private:
	int _current_flush_count; // how long ago we last flushed
};

extern Logging* g_log;