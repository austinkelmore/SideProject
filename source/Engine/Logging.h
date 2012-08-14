
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
			FlushFrequency = 15;
		}
		int FlushFrequency;
	DEFINE_CONFIG_END(Log)

	Logging();
	~Logging();

	void Log( LogChannel channel, const char *format, ... );

private:
	int mCurrentFlushCount; // how long ago we last flushed
};

extern Logging* gLog;