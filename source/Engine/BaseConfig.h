
#pragma once

#include "jsoncpp/json/json.h"

enum ConfigMemberType
{
	CONFIGVAR_Bool,
	CONFIGVAR_Int,
	CONFIGVAR_Float,
	CONFIGVAR_String,
	CONFIGVAR_Config
};

namespace Config
{
	template <class T> ConfigMemberType GetType() { return CONFIGVAR_Config; }
	template <> inline ConfigMemberType GetType<bool>() { return CONFIGVAR_Bool; }
	template <> inline ConfigMemberType GetType<int>() { return CONFIGVAR_Int; }
	template <> inline ConfigMemberType GetType<float>() { return CONFIGVAR_Float; }
	template <> inline ConfigMemberType GetType<std::string>() { return CONFIGVAR_String; }
}

struct ConfigMember
{
	union
	{
		bool *_value_bool;
		int *_value_int;
		float *_value_float;
		std::string *_value_string;
		class BaseConfig *_value_config;
	};

	std::string _name;
	ConfigMemberType _type;

	void SetFromJSON(const Json::Value &json_values);
};

class BaseConfig
{
public:
	virtual ~BaseConfig();
	virtual char* GetClassName() const = 0;
	virtual char* GetJSONName() const = 0;
	virtual BaseConfig* New() const = 0;

	void InitializeMembers();
	void SetFromJSON(const Json::Value &json_values);
protected:
	void AddMember(const std::string &type_name, void* data, ConfigMemberType data_type);
private:
	virtual void DefineMembers() = 0;
	std::vector<ConfigMember> _members;
};

#define DEFINE_CONFIG(class_name, json_name) \
class class_name : public BaseConfig \
{ \
private: \
	static class_name* s_##class_name; \
	virtual void DefineMembers(); \
public: \
	virtual ~class_name() {} \
	virtual char* GetClassName() const { return #class_name; } \
	virtual char* GetJSONName() const { return #json_name; } \
	virtual BaseConfig* New() const; \
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
		temp->InitializeMembers(); \
		return temp; \
	} \
	class_name* class_name##::StaticGetClass() \
	{ \
		if (!s_##class_name) \
		{ \
			s_##class_name = new class_name(); \
			s_##class_name->InitializeMembers(); \
		} \
		return s_##class_name; \
	} \
	void class_name::DefineMembers()

#define ADD_CONFIG_VAR_CLASS(type, var_name) \
	DBG_ASSERT_MSG(Config::GetType<type>() == CONFIGVAR_Config, "Non-class configs require a default value.  Use ADD_CONFIG_VAR if you want a normal variable."); \
	AddMember(#var_name, &var_name, Config::GetType<type>());

#define ADD_CONFIG_VAR(type, var_name, default_value) \
	var_name = default_value; \
	AddMember(#var_name, &var_name, Config::GetType<type>())
