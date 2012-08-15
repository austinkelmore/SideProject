
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

void ConfigVar::AssignValue( const Json::Value &configValue )
{
	switch ( type )
	{
	case CONFIGVAR_Bool:
		*valueBool = configValue.get( configName, *valueBool ).asBool(); break;
	case CONFIGVAR_Int:
		*valueInt = configValue.get( configName, *valueInt ).asInt(); break;
	case CONFIGVAR_Float:
		*valueFloat = configValue.get( configName, *valueFloat ).asFloat(); break;
	case CONFIGVAR_String:
		*valueString = configValue.get( configName, *valueString ).asString(); break;
	case CONFIGVAR_Props:
		// todo: amcgee - hook up more complicated props here
		break;
	default:
		// ASSERT( false );
		break;
	}
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

JSONConfig::~JSONConfig()
{
#ifdef WIN32
	FindCloseChangeNotification( mFolderChangeNotification );
#endif // WIN32
}

void JSONConfig::ReadConfigFolder( const std::string &folderPath )
{
	mFolderPath = folderPath;
	// hook up the ability to monitor the file for changes
#ifdef WIN32
	DWORD flags = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;
	mFolderChangeNotification = FindFirstChangeNotificationW( nowide::convert(mFolderPath).c_str(), false, flags );
	if ( mFolderChangeNotification == INVALID_HANDLE_VALUE )
	{
		mFolderChangeNotification = NULL;
		gLog->Log( LOG_Config, "Error trying to watch the configs directory: %d", GetLastError() );
	}
#endif // WIN32

	ParseConfigs();
}

void JSONConfig::ParseConfigs()
{
#ifdef WIN32
	WIN32_FIND_DATA findFileData;
	HANDLE findHandle;

	// filter only the JSON files
	std::string name( mFolderPath );
	name += "\\*.json";

	// get our first file
	findHandle = FindFirstFileW( nowide::convert(name).c_str(), &findFileData );
	if ( findHandle != INVALID_HANDLE_VALUE )
	{
		// keep going until we run out of files
		do 
		{
			std::string filePath( mFolderPath );
			filePath += '\\';
			filePath += nowide::convert(findFileData.cFileName);

			Json::Reader reader;

			mConfigFiles.push_back( filePath );
			std::ifstream configFile( filePath, ifstream::in );
			bool successful = reader.parse( configFile, mConfigFiles.back().rootValue );
			configFile.close();

			if (!successful)
				gLog->Log( LOG_Config, "Failed to parse configuration: %s\n", reader.getFormattedErrorMessages().c_str() );
		} while( FindNextFileW( findHandle, &findFileData ) != 0 );

		FindClose( findHandle );
	}
#endif // WIN32
}

void JSONConfig::CheckForConfigFolderChanges()
{
#ifdef WIN32
	// tell this not to wait and just return right away
	DWORD waitValue = WaitForSingleObject( mFolderChangeNotification, 0 );
	if ( waitValue == WAIT_OBJECT_0 )
	{
		// clear out the configs and then reparse them
		mConfigFiles.clear();
		ParseConfigs();
		LinkValuesToVariables();

		// eat all of the rest of the notifications
		// some times multiple are sent for the same save operation
		// we don't need to reparse on them because we JUST did it
		DWORD waitValue;
		do 
		{
			FindNextChangeNotification( mFolderChangeNotification );
			waitValue = WaitForSingleObject( mFolderChangeNotification, 0 );
		} while ( waitValue == WAIT_OBJECT_0 );
	}
#endif // WIN32
}

void JSONConfig::DebugPrintJSONConfigs()
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
					var.AssignValue( propsValues );
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