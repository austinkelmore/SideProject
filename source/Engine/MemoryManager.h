
#pragma once

#include <stdlib.h>
#include <map>
#include "BasicMacros.h"

struct MemoryAllocation
{
	MemoryAllocation() : _size(0), _filename("default"), _line_number(0) { }
	MemoryAllocation(const unsigned int size, const char* filename, const int line_number)
		: _size(size), _filename(filename), _line_number(line_number)
	{
		// do nothing
	}
	unsigned int _size;
	const char* _filename;
	int _line_number;
};

class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();

	void AddAllocation(const void* ptr, const unsigned int size, const char* file, const int line);
	void RemoveAllocation(const void* ptr);
	void DebugOutputAllocations();

	// the entry point for the memory manager
	static void CreateMemoryManager();
	static MemoryManager* GetMemoryManager();

private:
	static MemoryManager* _memory_manager;

	std::map<const void*, MemoryAllocation> _allocations;
};

#define ENABLE_MEMORY_MANAGER

#ifdef ENABLE_MEMORY_MANAGER
	inline void* operator new(size_t size, const char* file, const int line)
	{
		void* ptr = malloc(size);
		MemoryManager::GetMemoryManager()->AddAllocation(ptr, size, file, line);
		return ptr;
	}
	inline void* operator new[](size_t size, const char* file, const int line)
	{
		void* ptr = malloc(size);
		MemoryManager::GetMemoryManager()->AddAllocation(ptr, size, file, line);
		return ptr;
	}

	inline void operator delete(void* ptr)
	{
		MemoryManager::GetMemoryManager()->RemoveAllocation(ptr);
		free(ptr);
	}
	inline void operator delete(void* ptr, const char*, const int) throw()
	{
		MemoryManager::GetMemoryManager()->RemoveAllocation(ptr);
		free(ptr);
	}
	inline void operator delete[](void* ptr)
	{
		MemoryManager::GetMemoryManager()->RemoveAllocation(ptr);
		free(ptr);
	}
	inline void operator delete[](void* ptr, const char*, const int) throw()
	{
		MemoryManager::GetMemoryManager()->RemoveAllocation(ptr);
		free(ptr);
	}
#endif // ENABLE_MEMORY_MANAGER

#ifdef ENABLE_MEMORY_MANAGER
	#define DBG_NEW new(__FILE__, __LINE__)
#else
	#define DBG_NEW new
#endif // ENABLE_MEMORY_MANAGER

#define new DBG_NEW
