
#include "jsoncpp/json/json.h"
#include <nowide/convert.h>

class JSONConfig
{
public:
	void ReadConfig( const std::string &path );

	void DebugPrintValueStream();

private:
	Json::Value mRootValue;

	void InternalPrintValue( Json::Value &value, const std::string &path="." );
};

extern JSONConfig* gConfig;