
#pragma once

#include "jsoncpp/json/json.h"
#include <nowide/convert.h>
#include <string>

#ifdef WIN32
	#include <windows.h>
#endif // WIN32

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

#define ADD_PROPS( xType, xVarName ) g_config->AddNewProps( this, #xVarName, &xVarName, g_config->GetType<xType>() )

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
		bool *_value_bool;
		int *_value_int;
		float *_value_float;
		std::string *_value_string;
		BaseProps *_value_props;
	};

	std::string _config_name;
	ConfigVarType _type;

	void AssignValue( const Json::Value &config_value );
};

typedef std::vector<ConfigVar> ConfigVarList;

struct PropsData
{
	PropsData() : _static_init_callback(NULL) { }
	ConfigVarList _config_vars;
	std::string _props_name;
	void (*_static_init_callback)(void);
};

struct ConfigFileData
{
	ConfigFileData() {}
	ConfigFileData(const std::string &name) : _config_name(name) {}

	std::string _config_name;
	Json::Value _root_value;
};

typedef std::map<BaseProps*, PropsData> tPropsToDataMap;
typedef std::vector<ConfigFileData> tConfigFileVector;

class JSONConfig
{
public:
	static JSONConfig* GetConfigManager();

	JSONConfig();
	~JSONConfig();

	void ReadConfigFolder( const std::string &folder_path );
	void CheckForConfigFolderChanges();

	void DebugPrintJSONConfigs();

	void AddNewProps( BaseProps *props, const std::string &type_name, void* data, ConfigVarType data_type );
	void AddPropsCallback( BaseProps *props );

	void Initialize();

	template <class T> ConfigVarType GetType() { return CONFIGVAR_Props; }
	template <> ConfigVarType GetType<bool>() { return CONFIGVAR_Bool; }
	template <> ConfigVarType GetType<int>() { return CONFIGVAR_Int; }
	template <> ConfigVarType GetType<float>() { return CONFIGVAR_Float; }
	template <> ConfigVarType GetType<std::string>() { return CONFIGVAR_String; }

private:
	tConfigFileVector _config_files;
	tPropsToDataMap _props_to_data_map;
	std::string _folder_path;

#ifdef WIN32
	HANDLE _folder_change_notification;
#endif // WIN32

	void InternalPrintValue( Json::Value &value, const std::string &path="." );
	void LinkValuesToVariables();
	void ParseConfigs();
};

extern JSONConfig* g_config;