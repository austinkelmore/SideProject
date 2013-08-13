 
#include "JSONConfig.h"
#include "Logging.h"
#include "FileIO.h"
#include <nowide/convert.h>
#include <fstream>
#include <iostream>
#include <algorithm>

JSONConfig* g_config = NULL;

using std::ifstream;

void ConfigVar::AssignValue(const Json::Value &config_value)
{
	switch (_type)
	{
	case CONFIGVAR_Bool:
		*_value_bool = config_value.get(_name, *_value_bool).asBool(); break;
	case CONFIGVAR_Int:
		*_value_int = config_value.get(_name, *_value_int).asInt(); break;
	case CONFIGVAR_Float:
		*_value_float = config_value.get(_name, *_value_float).asFloat(); break;
	case CONFIGVAR_String:
		*_value_string = config_value.get(_name, *_value_string).asString(); break;
	case CONFIGVAR_Config:
		// todo: amcgee - hook up more complicated configs here
		//SetupConfig()
		break;
	default:
		DBG_ASSERT_FAIL("Unknown ConfigVar Type");
		break;
	}
}

JSONConfig::JSONConfig()
{
	_folder_change_notification = NULL;
}

JSONConfig::~JSONConfig()
{
	UnwatchFolder(_folder_change_notification);

	// go through and delete all of the configs we created
	for (tConfigToDataMap::iterator config = _config_to_data_map.begin(); config != _config_to_data_map.end(); ++config)
	{
		delete config->first;
	}
}

void JSONConfig::ReadConfigFolder(const std::string &folder_path)
{
	_folder_path = folder_path;

	// hook up the ability to monitor the file for changes
	if (!WatchFolder(folder_path.c_str(), _folder_change_notification))
		Logging::Log(LOG_Config, "Error trying to watch the configs directory: %d", GetLastError());

	ParseConfigs();
}

void JSONConfig::ParseConfigs()
{
	PrevFileHandle prev_handle = NULL;
	std::string file_path;

	do
	{
		file_path = FindFileInFolder(_folder_path.c_str(), prev_handle, "*.json");
		if (!file_path.empty())
		{
			Json::Reader reader;

			_config_files.push_back(file_path);

			char* file_stream = NULL;
			int file_size;
			if (LoadFile(file_path.c_str(), &file_stream, file_size))
			{
				if (!reader.parse(file_stream, file_stream + file_size, _config_files.back()._json_root))
					Logging::Log(LOG_Config, "Failed to parse configuration: %s\n", reader.getFormattedErrorMessages().c_str());

				CloseFile(file_stream);
			}
		}
	} while(!file_path.empty());
}

void JSONConfig::CheckForConfigFolderChanges()
{
	if (CheckForFolderChanges(_folder_change_notification))
	{
		// clear out the configs and then reparse them
		_config_files.clear();
		ParseConfigs();
		LinkValuesToVariables();
	}
}

void JSONConfig::DebugPrintJSONConfigs()
{
	for (tConfigFileVector::iterator configs = _config_files.begin(); configs != _config_files.end(); ++configs)
	{
		Logging::Log(LOG_Config, "Config File: %s\n", configs->_file_name.c_str());
		InternalPrintValue(configs->_json_root);
	}
}

void JSONConfig::LinkValuesToVariables()
{
	for (tConfigToDataMap::iterator config = _config_to_data_map.begin(); config != _config_to_data_map.end(); ++config)
	{
		for (tConfigFileVector::iterator file = _config_files.begin(); file != _config_files.end(); ++file)
		{
			Json::Value json_values = file->_json_root[config->second._name];

			ConfigData &data = config->second;
			for (unsigned i = 0; i < data._vars.size(); ++i)
			{
				ConfigVar &var = data._vars[i];
				if (json_values.isMember(var._name))
					var.AssignValue(json_values);
			}
		}
	}
}

void JSONConfig::SetupConfig(BaseConfig *config)
{
	ConfigData& config_data = _config_to_data_map[config];

	config_data._name = config->GetName();
	config->InitConfig();

	// read the config and set the values on the variables
	for (tConfigFileVector::iterator file = _config_files.begin(); file != _config_files.end(); ++file)
	{
		Json::Value json_values = file->_json_root[config_data._name];
		SetupConfig(config_data, json_values);
	}
}

void JSONConfig::SetupConfig(ConfigData& config_data, Json::Value json_values)
{
	for (unsigned i = 0; i < config_data._vars.size(); ++i)
	{
		ConfigVar &var = config_data._vars[i];
		if (json_values.isMember(var._name))
		{
			if (var._type != CONFIGVAR_Config)
				var.AssignValue(json_values);
			else
			{
				//ConfigData sub_config_data = _config_to_data_map[]
			}
		}
	}
}

void JSONConfig::AddNewConfigVariable(BaseConfig *config, const std::string &type_name, void* data, ConfigVarType data_type)
{
	ConfigVar temp;
	temp._name = type_name;
	temp._type = data_type;

	// it doesn't matter which we use since it's a union and they're all pointers
	temp._value_int = static_cast<int*>(data);

	_config_to_data_map[config]._vars.push_back(temp);
}

void JSONConfig::InternalPrintValue(Json::Value &value, const std::string &path/*="."*/)
{
	switch (value.type())
	{
	case Json::nullValue:
		Logging::Log(LOG_Config, "%s=null\n", path.c_str());
		break;
	case Json::intValue:
		Logging::Log(LOG_Config, "%s=%s\n", path.c_str(), Json::valueToString(value.asLargestInt()).c_str());
		break;
	case Json::uintValue:
		Logging::Log(LOG_Config, "%s=%s\n", path.c_str(), Json::valueToString(value.asLargestUInt()).c_str());
		break;
	case Json::realValue:
		Logging::Log(LOG_Config, "%s=%.16g\n", path.c_str(), value.asDouble());
		break;
	case Json::stringValue:
		Logging::Log(LOG_Config, "%s=\"%s\"\n", path.c_str(), value.asString().c_str());
		break;
	case Json::booleanValue:
		Logging::Log(LOG_Config, "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false");
		break;
	case Json::arrayValue:
		{
			Logging::Log(LOG_Config, "%s=[]\n", path.c_str());
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
			Logging::Log(LOG_Config, "%s={}\n", path.c_str());
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