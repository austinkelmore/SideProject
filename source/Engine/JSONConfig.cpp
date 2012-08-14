
#include "JSONConfig.h"
#include "Logging.h"
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef WIN32
	#include <windows.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <tchar.h>
#endif // WIN32

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

JSONConfig::JSONConfig()
{
	mFolderChangeNotification = NULL;
}

void JSONConfig::ReadConfigFolder( const std::string &folderPath )
{
	// hook up the ability to monitor the file for changes
#ifdef WIN32
	DWORD flags = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;
	mFolderChangeNotification = FindFirstChangeNotificationW( nowide::convert(folderPath).c_str(), false, flags );

	WIN32_FIND_DATA findFileData;
	HANDLE findHandle;

	// filter only the JSON files
	std::string name( folderPath );
	name += "\\*.json";

	// get our first file
	findHandle = FindFirstFileW( nowide::convert(name).c_str(), &findFileData );
	if ( findHandle != INVALID_HANDLE_VALUE )
	{
		// keep going until we run out of files
		do 
		{
			std::string filePath( folderPath );
			filePath += '\\';
			filePath += nowide::convert(findFileData.cFileName);

			Json::Reader reader;

			mConfigFiles.push_back( filePath );
			std::ifstream configFile( filePath, ifstream::in );
			bool successful = reader.parse( configFile, mConfigFiles.back().rootValue );

			if (!successful)
				gLog->Log( LOG_Config, "Failed to parse configuration: %s\n", reader.getFormattedErrorMessages() );
		} while( FindNextFileW( findHandle, &findFileData ) != 0 );

		FindClose( findHandle );
	}
#endif // WIN32
}

void JSONConfig::DebugPrintValueStream()
{
	for ( tConfigFileVector::iterator configs = mConfigFiles.begin(); configs != mConfigFiles.end(); ++configs )
	{
		gLog->Log( LOG_Config, "Config File: %s\n", configs->configName.c_str() );
		InternalPrintValue( configs->rootValue );
	}
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
		for ( tConfigFileVector::iterator configs = mConfigFiles.begin(); configs != mConfigFiles.end(); ++configs )
		{
			Json::Value propsValues = configs->rootValue[props->second.propsName];

			PropsData &data = props->second;
			for (unsigned i = 0; i < data.configVars.size(); ++i)
			{
				ConfigVar &var = data.configVars[i];
				if ( propsValues.isMember( var.configName ) )
				{
					// todo: amcgee - figure out a good way to handle all of the values
					Json::Value configValue = propsValues.get( var.configName, *var.valueInt );
					*var.valueInt = configValue.asInt();
				}
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