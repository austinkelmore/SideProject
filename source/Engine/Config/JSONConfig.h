
#pragma once

#include "jsoncpp/json/json.h"
#include <nowide/convert.h>
#include <string>

class BaseProps
{
public:
	BaseProps();
	virtual BaseProps* New() = 0;
	virtual char* GetName() = 0;
	virtual void StaticInitProps() = 0;
};

#define DEFINE_CONFIG( xConfigName ) \
class xConfigName##Props : public BaseProps \
{ \
public: \
	BaseProps* New() { return new xConfigName##Props(); } \
	char* GetName() { return #xConfigName; } \
	void StaticInitProps(); \
	xConfigName##Props() : BaseProps()

#define DEFINE_CONFIG_END( xConfigName ) \
}; \
xConfigName##Props* GetProps();

#define IMPLEMENT_CONFIG( xConfigName, xClassName ) \
static xClassName::xConfigName##Props s_##xConfigName##Props; \
xClassName::xConfigName##Props* xClassName::GetProps() { return &s_##xConfigName##Props; } \
void xClassName::xConfigName##Props::StaticInitProps()

#define ADD_PROPS( xType, xVarName ) gConfig->AddNewProps( this, #xVarName, &xVarName, gConfig->GetType<xType>() )

enum ConfigVarType
{
	CONFIGVAR_Bool,
	CONFIGVAR_Int,
	CONFIGVAR_Float,
	CONFIGVAR_String,
	CONFIGVAR_Props
};

struct ConfigVar
{
	union
	{
		bool *valueBool;
		int *valueInt;
		float *valueFloat;
		std::string *valueString;
		BaseProps *valueProps;
	};

	std::string configName;

	ConfigVarType type;
};

typedef std::vector<ConfigVar> ConfigVarList;

struct PropsData
{
	PropsData() : staticInitCallback(NULL) { }
	ConfigVarList configVars;
	std::string propsName;
	void (*staticInitCallback)(void);
};

typedef std::map<BaseProps*, PropsData> tPropsToDataMap;

class JSONConfig
{
public:
	static JSONConfig* GetConfigManager();

	void ReadConfig( const std::string &path );

	void DebugPrintValueStream();

	void AddNewProps( BaseProps *props, const std::string &typeName, void* data, ConfigVarType dataType );
	void AddPropsCallback( BaseProps *props );

	void Initialize();

	template <class T> ConfigVarType GetType() { return CONFIGVAR_Props; }
	template <> ConfigVarType GetType<bool>() { return CONFIGVAR_Bool; }
	template <> ConfigVarType GetType<int>() { return CONFIGVAR_Int; }
	template <> ConfigVarType GetType<float>() { return CONFIGVAR_Float; }
	template <> ConfigVarType GetType<std::string>() { return CONFIGVAR_String; }

private:
	Json::Value mRootValue;
	tPropsToDataMap mPropsToPropsDataMap;

	void InternalPrintValue( Json::Value &value, const std::string &path="." );
	void LinkValuesToVariables();
};

extern JSONConfig* gConfig;