
#pragma once

#ifdef WIN32
#include <windows.h>

typedef HANDLE FolderChangeNotificationHandle;
typedef HANDLE PrevFileHandle;
#endif

bool LoadFile(const char* filename, void** out_data, int &out_size);

bool WatchFolder(const char* folder_path, FolderChangeNotificationHandle &out_handle);
bool CheckForFolderChanges(const FolderChangeNotificationHandle handle);
void UnwatchFolder(const FolderChangeNotificationHandle handle);

std::string FindFileInFolder(const char* folder_path, PrevFileHandle &out_prev_handle, const char* file_wildcard);
