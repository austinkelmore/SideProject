
#include "JSONConfig.h"
#include "../Logging/Logging.h"
#include <fstream>
#include <iostream>
#include <algorithm>

JSONConfig* gConfig = NULL;

using std::ifstream;

BaseProps::BaseProps()
{
	// todo: amcgee - figure out how to make this not alloc memory in a constructor
	JSONConfig::GetConfigManager()->AddPropsCallback( this );
}

JSONConfig* JSONConfig::GetConfigManager()
{
	if ( gConfig )
		return gConfig;
	else
	{
		gConfig = new JSONConfig();
		return gConfig;
	}
}

void JSONConfig::ReadConfig( const std::string &path )
{
	// do something
	Json::Reader reader;

	std::ifstream configFile( path, ifstream::in );
	bool successfull = reader.parse( configFile, mRootValue );

	if ( !successfull )
	{
		std::cout << "Failed to parse configuration:" << reader.getFormattedErrorMessages();
	}
}

void JSONConfig::DebugPrintValueStream()
{
	InternalPrintValue( mRootValue );
}

void JSONConfig::Initialize()
{
	for ( tPropsToDataMap::iterator i = mPropsToPropsDataMap.begin(); i != mPropsToPropsDataMap.end(); ++i )
	{
		i->second.propsName = i->first->GetName();
		i->first->StaticInitProps();
	}

	// now that we know of all the props, let's hook them up to their values
	LinkValuesToVariables();
}

void JSONConfig::LinkValuesToVariables()
{
	for ( tPropsToDataMap::iterator props = mPropsToPropsDataMap.begin(); props != mPropsToPropsDataMap.end(); ++props )
	{
		Json::Value propsValues = mRootValue[props->second.propsName];

		PropsData &data = props->second;
		for (unsigned i = 0; i < data.configVars.size(); ++i)
		{
			ConfigVar &var = data.configVars[i];
			if ( propsValues.isMember( var.configName ) )
			{
				Json::Value configValue = propsValues.get( var.configName, *var.valueInt );
				*var.valueInt = configValue.asInt();
			}
		}
	}
}

void JSONConfig::AddPropsCallback( BaseProps *props )
{
	mPropsToPropsDataMap[props];
}

void JSONConfig::AddNewProps( BaseProps *props, const std::string &typeName, void* data, ConfigVarType dataType )
{
	ConfigVar temp;
	temp.configName = typeName;
	temp.type = dataType;

	// it doesn't matter which we use since it's a union an they're all pointers
	temp.valueInt = static_cast<int*>( data );

	mPropsToPropsDataMap[props].configVars.push_back( temp );
}

void JSONConfig::InternalPrintValue( Json::Value &value, const std::string &path/*="."*/ )
{
	switch ( value.type() )
	{
	case Json::nullValue:
		gLog->Log( LOG_Engine, "%s=null\n", path.c_str() );
		break;
	case Json::intValue:
		gLog->Log( LOG_Engine, "%s=%s\n", path.c_str(), Json::valueToString( value.asLargestInt() ).c_str() );
		break;
	case Json::uintValue:
		gLog->Log( LOG_Engine, "%s=%s\n", path.c_str(), Json::valueToString( value.asLargestUInt() ).c_str() );
		break;
	case Json::realValue:
		gLog->Log( LOG_Engine, "%s=%.16g\n", path.c_str(), value.asDouble() );
		break;
	case Json::stringValue:
		gLog->Log( LOG_Engine, "%s=\"%s\"\n", path.c_str(), value.asString().c_str() );
		break;
	case Json::booleanValue:
		gLog->Log( LOG_Engine, "%s=%s\n", path.c_str(), value.asBool() ? "true" : "false" );
		break;
	case Json::arrayValue:
		{
			gLog->Log( LOG_Engine, "%s=[]\n", path.c_str() );
			int size = value.size();
			for ( int index =0; index < size; ++index )
			{
				static char buffer[16];
				sprintf_s( buffer, 16, "[%d]", index );
				InternalPrintValue( value[index], path + buffer );
			}
		}
		break;
	case Json::objectValue:
		{
			gLog->Log( LOG_Engine, "%s={}\n", path.c_str() );
			Json::Value::Members members( value.getMemberNames() );
			std::sort( members.begin(), members.end() );
			std::string suffix = *(path.end()-1) == '.' ? "" : ".";
			for ( Json::Value::Members::iterator it = members.begin(); 
				it != members.end(); 
				++it )
			{
				const std::string &name = *it;
				InternalPrintValue( value[name], path + suffix + name );
			}
		}
		break;
	default:
		break;
	}
}