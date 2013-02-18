
#include "FileIO.h"
#include <nowide/convert.h>

bool LoadFile(const char* filename, void** out_data, int &out_size)
{
	DBG_ASSERT(filename);
	DBG_ASSERT(out_data);

#ifdef WIN32
	FILE *file_stream = NULL;
	if (_wfopen_s(&file_stream, nowide::convert(filename).c_str(), L"rb") == 0)
	{
		// find out how big the file is
		fseek(file_stream, 0, SEEK_END);
		out_size = ftell(file_stream);
		fseek(file_stream, 0, SEEK_SET);

		// create a buffer for the file and read in the data
		char* file_data = new char[out_size];
		size_t success = _fread_nolock(file_data, 1, out_size, file_stream);
		DBG_ASSERT((int)success == out_size);

		*out_data = file_data;
		fclose(file_stream);

		return true;
	}
#endif // WIN32

	return false;
}

bool WatchFolder(const char* folder_path, FolderChangeNotificationHandle &out_handle)
{
#ifdef WIN32
	DWORD flags = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME;
	out_handle = FindFirstChangeNotificationW(nowide::convert(folder_path).c_str(), false, flags);
	if (out_handle == INVALID_HANDLE_VALUE)
	{
		out_handle = NULL;
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

std::string FindFileInFolder(const char* folder_path, PrevFileHandle &out_prev_handle, const char* file_wildcard)
{
	std::string folder_wildcard(folder_path);
	if (file_wildcard)
	{
		folder_wildcard += "\\";
		folder_wildcard += file_wildcard;
	}

#ifdef WIN32
	WIN32_FIND_DATA find_file_data;

	if (out_prev_handle == NULL)
	{
		// get our first file
		out_prev_handle = FindFirstFileW(nowide::convert(folder_wildcard).c_str(), &find_file_data);
		if (out_prev_handle == INVALID_HANDLE_VALUE)
			return "";
	}
	else if (!FindNextFileW(out_prev_handle, &find_file_data))
	{
		FindClose(out_prev_handle);
		return "";
	}
#endif // WIN32

	// if we've made it here, we have a valid file
	std::string filePath(folder_path);
	filePath += '\\';
	filePath += nowide::convert(find_file_data.cFileName);

	return filePath;
}
