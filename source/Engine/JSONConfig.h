
#pragma once

#include "jsoncpp/json/json.h"
#include <string>
#include "BasicMacros.h"
#include "FileIO.h"

class BaseConfig
{
public:
	virtual ~BaseConfig() {}
	virtual char* GetName() = 0;
	virtual void InitConfig() = 0;
};

#define DEFINE_CONFIG(xConfigName) \
	class xConfigName##Config; \
	xConfigName##Config* GetConfig(); \
	class xConfigName##Config : public BaseConfig \
	{ \
	public: \
		virtual ~xConfigName##Config() {} \
		virtual char* GetName() { return #xConfigName; } \
		virtual void InitConfig();

#define DEFINE_CONFIG_END \
	};

#define IMPLEMENT_CONFIG(xConfigName, xClassName) \
xClassName##::##xConfigName##Config* xClassName##::GetConfig() \
{ \
	static xConfigName##Config* s_##xConfigName##Config = NULL; \
	if (!s_##xConfigName##Config) \
	{ \
		s_##xConfigName##Config = new xConfigName##Config(); \
		DBG_ASSERT_MSG(g_config, "Config Manager not created."); \
		g_config->SetupConfig(s_##xConfigName##Config); \
	} \
	return s_##xConfigName##Config; \
} \
void xClassName##::##xConfigName##Config::InitConfig()

#define ADD_CONFIG_VAR(xType, xVarName, xDefaultValue) \
	xVarName = xDefaultValue; \
	g_config->AddNewConfigVariable(this, #xVarName, &xVarName, g_config->GetType<xType>())

enum ConfigVarType
{
	CONFIGVAR_Bool,
	CONFIGVAR_Int,
	CONFIGVAR_Float,
	CONFIGVAR_String,
	CONFIGVAR_Config
};

struct ConfigVar
{
	union
	{
		bool *_value_bool;
		int *_value_int;
		float *_value_float;
		std::string *_value_string;
		BaseConfig *_value_config;
	};

	std::string _name;
	ConfigVarType _type;

	void AssignValue(const Json::Value &json_value);
};

struct ConfigData
{
	std::vector<ConfigVar> _vars;
	std::string _name;
};

struct ConfigFileData
{
	ConfigFileData() {}
	ConfigFileData(const std::string &file_name) : _file_name(file_name) {}

	std::string _file_name;
	Json::Value _json_root;
};

typedef std::map<BaseConfig*, ConfigData> tConfigToDataMap;
typedef std::vector<ConfigFileData> tConfigFileVector;

class JSONConfig
{
public:
	JSONConfig();
	~JSONConfig();

	void ReadConfigFolder(const std::string &folder_path);
	void CheckForConfigFolderChanges();

	void DebugPrintJSONConfigs();

	void AddNewConfigVariable(BaseConfig *config, const std::string &type_name, void* data, ConfigVarType data_type);
	void SetupConfig(BaseConfig *config);
	void SetupConfig(ConfigData& config_data, Json::Value json_values);

	template <class T> ConfigVarType GetType() { return CONFIGVAR_Config; }
	template <> ConfigVarType GetType<bool>() { return CONFIGVAR_Bool; }
	template <> ConfigVarType GetType<int>() { return CONFIGVAR_Int; }
	template <> ConfigVarType GetType<float>() { return CONFIGVAR_Float; }
	template <> ConfigVarType GetType<std::string>() { return CONFIGVAR_String; }

private:
	tConfigFileVector _config_files;
	tConfigToDataMap _config_to_data_map;
	std::string _folder_path;

	FolderChangeNotificationHandle _folder_change_notification;

	void InternalPrintValue(Json::Value &value, const std::string &path=".");
	void LinkValuesToVariables();
	void ParseConfigs();
};

extern JSONConfig* g_config;