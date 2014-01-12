
#pragma once

#include "jsoncpp/json/json.h"
#include <string>
#include "BasicMacros.h"
#include "FileIO.h"

// the path configs take to get created and initialized is:
// 1. The Config is created with ConfigPtr->New();
// 2. That calls InitializeConfig on the JSONConfigMgr
// 3. That fills the ConfigData with the Name and calls DefineMembers on the config itself
// 4. DefineMembers is where you add all of the variables so that we can have reflection
// 5. After that InitializeConfig is called and it tries to pull all of the values from the json data
//	  and fill it into the ConfigData struct (which holds the pointers to the variables in the Config)

struct JSONFileData
{
	JSONFileData() {}
	JSONFileData(const std::string &file_name) : _file_name(file_name) {}

	std::string _file_name;
	Json::Value _json_root;
};

class JSONConfigMgr
{
public:
	JSONConfigMgr();
	~JSONConfigMgr();

	void ReadConfigFolder(const std::string &folder_path);
	void CheckForConfigFolderChanges();

	void DebugPrintJSONConfigs();

	void InitializeConfig(class BaseConfig *config, const bool add_to_mgr);
	void RemoveFromMgr(const class BaseConfig *config);

private:
	std::vector<JSONFileData> _json_files;
	std::vector<class BaseConfig*> _configs;
	std::string _folder_path;

	FolderChangeNotificationHandle _folder_change_notification;

	void InternalPrintValue(Json::Value &value, const std::string &path=".");
	void RefreshConfigValuesFromJSON();
	void ParseConfigs();
};

extern JSONConfigMgr* g_config;