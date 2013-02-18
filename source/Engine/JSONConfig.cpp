 
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
		*_value_bool = config_value.get(_config_name, *_value_bool).asBool(); break;
	case CONFIGVAR_Int:
		*_value_int = config_value.get(_config_name, *_value_int).asInt(); break;
	case CONFIGVAR_Float:
		*_value_float = config_value.get(_config_name, *_value_float).asFloat(); break;
	case CONFIGVAR_String:
		*_value_string = config_value.get(_config_name, *_value_string).asString(); break;
	case CONFIGVAR_Props:
		// todo: amcgee - hook up more complicated props here
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
	for (tPropsToDataMap::iterator config = _props_to_data_map.begin(); config != _props_to_data_map.end(); ++config)
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

			// todo: amcgee - clean up the delete aspect of the file_stream, I don't like how this is done
			char* file_stream = NULL;
			int file_size;
			if (LoadFile(file_path.c_str(), (void**)&file_stream, file_size))
			{
				if (!reader.parse(file_stream, file_stream + file_size, _config_files.back()._root_value))
					Logging::Log(LOG_Config, "Failed to parse configuration: %s\n", reader.getFormattedErrorMessages().c_str());

				delete[] file_stream;
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
		Logging::Log(LOG_Config, "Config File: %s\n", configs->_config_name.c_str());
		InternalPrintValue(configs->_root_value);
	}
}

void JSONConfig::LinkValuesToVariables()
{
	for (tPropsToDataMap::iterator props = _props_to_data_map.begin(); props != _props_to_data_map.end(); ++props)
	{
		for (tConfigFileVector::iterator configs = _config_files.begin(); configs != _config_files.end(); ++configs)
		{
			Json::Value props_values = configs->_root_value[props->second._props_name];

			PropsData &data = props->second;
			for (unsigned i = 0; i < data._config_vars.size(); ++i)
			{
				ConfigVar &var = data._config_vars[i];
				if (props_values.isMember(var._config_name))
					var.AssignValue(props_values);
			}
		}
	}
}

void JSONConfig::SetupProps(BaseProps *props)
{
	PropsData& propsData = _props_to_data_map[props];

	propsData._props_name = props->GetName();
	props->InitProps();

	// read the config and set the values on the variables
	for (tConfigFileVector::iterator configs = _config_files.begin(); configs != _config_files.end(); ++configs)
	{
		Json::Value props_values = configs->_root_value[propsData._props_name];

		for (unsigned i = 0; i < propsData._config_vars.size(); ++i)
		{
			ConfigVar &var = propsData._config_vars[i];
			if (props_values.isMember(var._config_name))
				var.AssignValue(props_values);
		}
	}
}

void JSONConfig::AddNewPropsVariable(BaseProps *props, const std::string &type_name, void* data, ConfigVarType data_type)
{
	ConfigVar temp;
	temp._config_name = type_name;
	temp._type = data_type;

	// it doesn't matter which we use since it's a union an they're all pointers
	temp._value_int = static_cast<int*>(data);

	_props_to_data_map[props]._config_vars.push_back(temp);
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