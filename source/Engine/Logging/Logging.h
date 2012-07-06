
enum LogChannel
{
	LOG_UI = 0,
	LOG_Gameplay,
	LOG_Graphics,
	LOG_Physics,
	LOG_Input,
	LOG_FileIO,
	LOG_Network,
	LOG_Engine
};

enum LogTarget
{
	LOGTARGET_STDOUT,
	LOGTARGET_FILE
};

class Logging
{
public:
	Logging();
	~Logging();

	void Log( LogChannel channel, const char *format, ... );
	void SetFlushFrequency( const int frequency ) { /*ASSERT(frequency >= 0);*/ mFlushFrequency = frequency; }

private:
	int mFlushFrequency; // how often we flush the buffer
	int mCurrentFlushCount; // how long ago we last flushed
};

extern Logging* gLog;