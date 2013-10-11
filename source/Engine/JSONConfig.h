
#pragma once

#include "jsoncpp/json/json.h"
#include <string>
#include "BasicMacros.h"
#include "FileIO.h"

// the path configs take to get created and initialized is:
// 1. The Config is created with ConfigPtr->New();
// 2. That calls InitializeConfig on the JSONConfig
// 3. That fills the ConfigData with the Name and calls InitConfig on the config itself
// 4. InitConfig is where you add all of the variables so that we can have reflection
// 5. After that InitializeConfig is called and it tries to pull all of the values from the json data
//	  and fill it into the ConfigData struct (which holds the 

class BaseConfig
{
public:
	virtual ~BaseConfig() {}
	virtual char* GetClassName() const = 0;
	virtual char* GetJSONName() const = 0;
	virtual BaseConfig* New() const = 0;
	virtual void InitConfig() = 0;
};

#define DEFINE_CONFIG(class_name, json_name) \
	class class_name : public BaseConfig \
	{ \
	private: \
		static class_name* s_##class_name; \
	public: \
		virtual ~class_name() {} \
		virtual char* GetClassName() const { return #class_name; } \
		virtual char* GetJSONName() const { return #json_name; } \
		virtual BaseConfig* New() const; \
		virtual void InitConfig(); \
		\
		static class_name* StaticNew(); \
		static class_name* StaticGetClass();

#define DEFINE_CONFIG_END \
	};

#define IMPLEMENT_CONFIG(class_name) \
class_name*	class_name::s_##class_name = NULL; \
BaseConfig* class_name##::New() const \
{ \
	return class_name::StaticNew(); \
} \
class_name* class_name##::StaticNew() \
{ \
	class_name* temp = new class_name(); \
	*temp = *StaticGetClass(); \
	return temp; \
} \
class_name* class_name##::StaticGetClass() \
{ \
	if (!s_##class_name) \
	{ \
		s_##class_name = new class_name(); \
		DBG_ASSERT_MSG(g_config, "Config Manager not created."); \
		g_config->InitializeConfig(s_##class_name); \
	} \
	return s_##class_name; \
} \
void class_name::InitConfig()

#define ADD_CONFIG_VAR_CLASS(type, var_name) \
	DBG_ASSERT_MSG(g_config->GetType<type>() == CONFIGVAR_Config, "Non-class configs require a default value.  Use ADD_CONFIG_VAR if you want a normal variable."); \
	g_config->AddNewConfigVariable(this, #var_name, &var_name, g_config->GetType<type>())

#define ADD_CONFIG_VAR(type, var_name, default_value) \
	var_name = default_value; \
	g_config->AddNewConfigVariable(this, #var_name, &var_name, g_config->GetType<type>())

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
	std::string _class_name;
	std::string _json_name;
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
	void InitializeConfig(BaseConfig *config);
	void InitializeConfig(ConfigData& config_data, Json::Value json_values);

	template <class T> ConfigVarType GetType() { return CONFIGVAR_Config; }
	template <> ConfigVarType GetType<bool>() { return CONFIGVAR_Bool; }
	template <> ConfigVarType GetType<int>() { return CONFIGVAR_Int; }
	template <> ConfigVarType GetType<float>() { return CONFIGVAR_Float; }
	template <> ConfigVarType GetType<std::string>() { return CONFIGVAR_String; }

private:
	tConfigFileVector _config_files;
	tConfigToDataMap _config_ptr_to_data_map;
	std::string _folder_path;

	FolderChangeNotificationHandle _folder_change_notification;

	void InternalPrintValue(Json::Value &value, const std::string &path=".");
	void RefreshConfigValuesFromJSON();
	void ParseConfigs();
};

extern JSONConfig* g_config;