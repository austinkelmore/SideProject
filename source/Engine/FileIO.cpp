
#include "FileIO.h"
#include <nowide/convert.h>

FileHandle LoadFile(const char* filename)
{
	return NULL;
}

void CloseFile(FileHandle handle)
{

}

bool WatchFolder(const char* folder_path, FolderChangeNotificationHandle &handle)
{
#ifdef WIN32
	DWORD flags = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;
	handle = FindFirstChangeNotificationW(nowide::convert(folder_path).c_str(), false, flags);
	if (handle == INVALID_HANDLE_VALUE)
	{
		handle = NULL;
		return false;
	}

	return true;
#endif // WIN32
}

bool CheckForFolderChanges(const FolderChangeNotificationHandle handle)
{
#ifdef WIN32
	// tell this not to wait and just return right away
	DWORD wait_value = WaitForSingleObject(handle, 0);
	if (wait_value == WAIT_OBJECT_0)
	{
		// eat all of the rest of the notifications
		// sometimes multiple are sent for the same save operation
		do 
		{
			FindNextChangeNotification(handle);
			wait_value = WaitForSingleObject(handle, 0);
		} while (wait_value == WAIT_OBJECT_0);

		return true;
	}

#endif // WIN32
	return false;
}

void UnwatchFolder(const FolderChangeNotificationHandle handle)
{
#ifdef WIN32
	FindCloseChangeNotification(handle);
#endif // WIN32
}

std::string FindFileInFolder(const char* folder_path, PrevFileHandle &prev_handle, const char* file_wildcard)
{
	std::string folder_wildcard(folder_path);
	if (file_wildcard)
	{
		folder_wildcard += "\\";
		folder_wildcard += file_wildcard;
	}

#ifdef WIN32
	WIN32_FIND_DATA find_file_data;

	if (prev_handle == NULL)
	{
		// get our first file
		prev_handle = FindFirstFileW(nowide::convert(folder_wildcard).c_str(), &find_file_data);
		if (prev_handle == INVALID_HANDLE_VALUE)
			return "";
	}
	else if (!FindNextFileW(prev_handle, &find_file_data))
	{
		FindClose(prev_handle);
		return "";
	}
#endif // WIN32

	// if we've made it here, we have a valid file
	std::string filePath(folder_path);
	filePath += '\\';
	filePath += nowide::convert(find_file_data.cFileName);

	return filePath;
}
