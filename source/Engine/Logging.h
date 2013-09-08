
#pragma once

#include "JSONConfig.h"

enum LogChannel
{
	LOG_Assert = 0,
	LOG_Config,
	LOG_FileIO,
	LOG_Gameplay,
	LOG_Graphics,
	LOG_Input,
	LOG_Memory,
	LOG_Network,
	LOG_Physics,
	LOG_Platform,
	LOG_UI,
};

enum LogTarget
{
	LOGTARGET_STDOUT,
	LOGTARGET_FILE
};

DEFINE_CONFIG(LogConfig, Log)
	int Flush_Frequency;
	bool Prepend_Group_Name;
DEFINE_CONFIG_END

class Logging
{
public:

	Logging();
	~Logging();

	static void Log(LogChannel channel, const char *format, ...);

	void EnableChannel(const LogChannel logChannel);
	void DisableChannel(const LogChannel logChannel);
	bool IsChannelEnabled(const LogChannel logChannel) const;
	const char* GetChannelString(const LogChannel logChannel) const;

	int GetFlushCount() { return _current_flush_count; }
	void SetFlushCount(const int flush_count) { _current_flush_count = flush_count; }

private:
	LogConfig* _log_config;
	int _current_flush_count; // how long ago we last flushed
	std::vector<LogChannel> _disabledChannels;
};

extern Logging* g_log;
