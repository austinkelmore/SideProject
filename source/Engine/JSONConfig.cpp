 
#include "JSONConfig.h"
#include "Logging.h"
#include "FileIO.h"
#include <nowide/convert.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "BaseConfig.h"

JSONConfigMgr* g_config = NULL;

//using std::ifstream;

JSONConfigMgr::JSONConfigMgr()
{
	_folder_change_notification = NULL;
}

JSONConfigMgr::~JSONConfigMgr()
{
	UnwatchFolder(_folder_change_notification);

	// go through and delete all of the configs we created
	for (unsigned i = 0; i < _configs.size(); ++i)
	{
		delete _configs[i];
	}
}

void JSONConfigMgr::ReadConfigFolder(const std::string &folder_path)
{
	_folder_path = folder_path;

	// hook up the ability to monitor the file for changes
	if (!WatchFolder(folder_path.c_str(), _folder_change_notification))
		Logging::Log(LOG_Config, "Error trying to watch the configs directory: %s with error %d", folder_path.c_str(), GetLastError());

	ParseConfigs();
}

void JSONConfigMgr::ParseConfigs()
{
	PrevFileHandle prev_handle = NULL;
	std::string file_path;

	do
	{
		file_path = FindFileInFolder(_folder_path.c_str(), prev_handle, "*.json");
		if (!file_path.empty())
		{
			Json::Reader reader;

			_json_files.push_back(file_path);

			char* file_stream = NULL;
			int file_size;
			if (LoadFileToBuffer(file_path.c_str(), &file_stream, file_size))
			{
				_json_files.back()._json_root.clear();
				if (!reader.parse(file_stream, file_stream + file_size, _json_files.back()._json_root))
					Logging::Log(LOG_Config, "Failed to parse configuration: %s", reader.getFormattedErrorMessages().c_str());

				DeleteFileBuffer(&file_stream);
			}
		}
	} while(!file_path.empty());
}

void JSONConfigMgr::CheckForConfigFolderChanges()
{
	if (CheckForFolderChanges(_folder_change_notification))
	{
		// clear out the configs and then reparse them
		_json_files.clear();
		ParseConfigs();
		RefreshConfigValuesFromJSON();
		DebugPrintJSONConfigs();
	}
}

void JSONConfigMgr::DebugPrintJSONConfigs()
{
	for (std::vector<JSONFileData>::iterator configs = _json_files.begin(); configs != _json_files.end(); ++configs)
	{
		Logging::Log(LOG_Config, "Config File: %s", configs->_file_name.c_str());
		InternalPrintValue(configs->_json_root);
	}
}

void JSONConfigMgr::RefreshConfigValuesFromJSON()
{
	for (unsigned i = 0; i < _configs.size(); ++i)
	{
		BaseConfig* config = _configs[i];
		InitializeConfig(config, false);
	}
}

void JSONConfigMgr::InitializeConfig(BaseConfig *config, const bool add_to_mgr)
{
	if (add_to_mgr)
	{
		DBG_ASSERT_MSG(std::find(_configs.begin(), _configs.end(), config) == _configs.end(), "Trying to initialize a config that already exists in the JSON Manager.");
		_configs.push_back(config);
	}

	// fill the configs from the files
	for (std::vector<JSONFileData>::iterator file = _json_files.begin(); file != _json_files.end(); ++file)
	{
		config->SetFromJSON(file->_json_root);
	}
}

void JSONConfigMgr::RemoveFromMgr(const BaseConfig *config)
{
	std::vector<BaseConfig*>::iterator iter = std::find(_configs.begin(), _configs.end(), config);
	DBG_ASSERT_MSG(iter != _configs.end(), "Trying to remove a config frmo the JSON Mgr that isn't tracked.");
	_configs.erase(iter);
}

void JSONConfigMgr::InternalPrintValue(Json::Value &value, const std::string &path/*="."*/)
{
	switch (value.type())
	{
	case Json::nullValue:
		Logging::Log(LOG_Config, "%s=null", path.c_str());
		break;
	case Json::intValue:
		Logging::Log(LOG_Config, "%s=%s", path.c_str(), Json::valueToString(value.asLargestInt()).c_str());
		break;
	case Json::uintValue:
		Logging::Log(LOG_Config, "%s=%s", path.c_str(), Json::valueToString(value.asLargestUInt()).c_str());
		break;
	case Json::realValue:
		Logging::Log(LOG_Config, "%s=%.16g", path.c_str(), value.asDouble());
		break;
	case Json::stringValue:
		Logging::Log(LOG_Config, "%s=\"%s\"", path.c_str(), value.asString().c_str());
		break;
	case Json::booleanValue:
		Logging::Log(LOG_Config, "%s=%s", path.c_str(), value.asBool() ? "true" : "false");
		break;
	case Json::arrayValue:
		{
			Logging::Log(LOG_Config, "%s=[]", path.c_str());
			int size = value.size();
			for (int index =0; index < size; ++index)
			{
				char buffer[16];
				sprintf_s(buffer, 16, "[%d]", index);
				InternalPrintValue(value[index], path + buffer);
			}
		}
		break;
	case Json::objectValue:
		{
			Logging::Log(LOG_Config, "%s={}", path.c_str());
			Json::Value::Members members(value.getMemberNames());
			std::sort(members.begin(), members.end());
			std::string suffix = *(path.end()-1) == '.' ? "" : ".";
			for (Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it)
			{
				const std::string &name = *it;
				InternalPrintValue(value[name], path + suffix + name);
			}
		}
		break;
	default:
		break;
	}
}