
#include "BaseConfig.h"
#include "JSONConfig.h"

void ConfigMember::SetFromJSON(const Json::Value &json_values)
{
	switch (_type)
	{
	case CONFIGVAR_Bool:
		*_value_bool = json_values.get(_name, *_value_bool).asBool(); break;
	case CONFIGVAR_Int:
		*_value_int = json_values.get(_name, *_value_int).asInt(); break;
	case CONFIGVAR_Float:
		*_value_float = json_values.get(_name, *_value_float).asFloat(); break;
	case CONFIGVAR_String:
		*_value_string = json_values.get(_name, *_value_string).asString(); break;
	case CONFIGVAR_Config:
		// todo: akelmore - hook up more complicated member types here
		//SetupConfig()
		break;
	default:
		DBG_ASSERT_FAIL("Unknown ConfigVar Type");
		break;
	}
}

BaseConfig::~BaseConfig()
{
	DBG_ASSERT_MSG(g_config, "Trying to delete a config when the JSON Mgr doesn't exist.");
	g_config->RemoveFromMgr(this);
}

void BaseConfig::InitializeMembers()
{
	DefineMembers();
	DBG_ASSERT_MSG(g_config, "Config Manager not created before trying to initialize a BaseConfig.");
	g_config->InitializeConfig(this, true);
}

void BaseConfig::SetFromJSON(const Json::Value &json_values)
{
	if (!json_values.empty())
	{
		Json::Value member_values = json_values[GetJSONName()];
		if (!member_values.empty())
		{
			for (unsigned i = 0; i < _members.size(); ++i)
				_members[i].SetFromJSON(member_values);
		}
	}
}

void BaseConfig::AddMember(const std::string &type_name, void* data, ConfigMemberType data_type)
{
	ConfigMember temp;
	temp._name = type_name;
	temp._type = data_type;

	// the config variables are created at run-time, so since this is only happening in init, we have to create the variable
	if (data_type == CONFIGVAR_Config)
		temp._value_config = static_cast<BaseConfig*>(data)->New();
	else
		// it doesn't matter which we use since it's a union and they're all pointers
		temp._value_int = static_cast<int*>(data);

	_members.push_back(temp);
}
