
#pragma once

#ifdef WIN32
#include <windows.h>

typedef FILE* FileHandle;
typedef HANDLE FolderChangeNotificationHandle;
typedef HANDLE PrevFileHandle;
#endif

FileHandle LoadFile(const char* filename);
void CloseFile(FileHandle handle);

bool WatchFolder(const char* folder_path, FolderChangeNotificationHandle &handle);
bool CheckForFolderChanges(const FolderChangeNotificationHandle handle);
void UnwatchFolder(const FolderChangeNotificationHandle handle);

std::string FindFileInFolder(const char* folder_path, PrevFileHandle &prev_handle, const char* file_wildcard);
