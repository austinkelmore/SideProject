
#pragma once

#include "jsoncpp/json/json.h"
#include <string>
#include "BasicMacros.h"
#include "FileIO.h"

class BaseProps
{
public:
	virtual ~BaseProps() {}
	virtual char* GetName() = 0;
	virtual void InitProps() = 0;
};

#define DEFINE_CONFIG(xConfigName) \
	class xConfigName##Props; \
	xConfigName##Props* GetProps(); \
	class xConfigName##Props : public BaseProps \
	{ \
	public: \
		virtual ~xConfigName##Props() {} \
		virtual char* GetName() { return #xConfigName; } \
		virtual void InitProps();

#define DEFINE_CONFIG_END() \
	};

#define IMPLEMENT_CONFIG(xConfigName, xClassName) \
xClassName##::##xConfigName##Props* xClassName##::GetProps() \
{ \
	static xConfigName##Props* s_##xConfigName##Props = NULL; \
	if (!s_##xConfigName##Props) \
	{ \
		s_##xConfigName##Props = new xConfigName##Props(); \
		DBG_ASSERT_MSG(g_config, "Config Manager not created."); \
		g_config->SetupProps(s_##xConfigName##Props); \
	} \
	return s_##xConfigName##Props; \
} \
void xClassName##::##xConfigName##Props::InitProps()

#define ADD_PROPS( xType, xVarName, xDefaultValue ) \
	xVarName = xDefaultValue; \
	g_config->AddNewPropsVariable(this, #xVarName, &xVarName, g_config->GetType<xType>())

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

	void AssignValue(const Json::Value &config_value);
};

typedef std::vector<ConfigVar> ConfigVarList;

struct PropsData
{
	ConfigVarList _config_vars;
	std::string _props_name;
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
	JSONConfig();
	~JSONConfig();

	void ReadConfigFolder(const std::string &folder_path);
	void CheckForConfigFolderChanges();

	void DebugPrintJSONConfigs();

	void AddNewPropsVariable(BaseProps *props, const std::string &type_name, void* data, ConfigVarType data_type);
	void SetupProps(BaseProps *props);

	template <class T> ConfigVarType GetType() { return CONFIGVAR_Props; }
	template <> ConfigVarType GetType<bool>() { return CONFIGVAR_Bool; }
	template <> ConfigVarType GetType<int>() { return CONFIGVAR_Int; }
	template <> ConfigVarType GetType<float>() { return CONFIGVAR_Float; }
	template <> ConfigVarType GetType<std::string>() { return CONFIGVAR_String; }

private:
	tConfigFileVector _config_files;
	tPropsToDataMap _props_to_data_map;
	std::string _folder_path;

	FolderChangeNotificationHandle _folder_change_notification;

	void InternalPrintValue(Json::Value &value, const std::string &path=".");
	void LinkValuesToVariables();
	void ParseConfigs();
};

extern JSONConfig* g_config;