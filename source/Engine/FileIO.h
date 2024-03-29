
#pragma once

#ifdef WIN32
#include <windows.h>

typedef HANDLE FolderChangeNotificationHandle;
typedef HANDLE PrevFileHandle;
#endif // WIN32

bool LoadFileToBuffer(const char* filename, char** out_data, int &out_size);
void DeleteFileBuffer(char** data_buffer);

bool WatchFolder(const char* folder_path, FolderChangeNotificationHandle &out_handle);
bool CheckForFolderChanges(const FolderChangeNotificationHandle handle);
void UnwatchFolder(const FolderChangeNotificationHandle handle);

std::string FindFileInFolder(const char* folder_path, PrevFileHandle &out_prev_handle, const char* file_wildcard);
