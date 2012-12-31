
#include "MemoryManager.h"
#include "Logging.h"

MemoryManager* MemoryManager::_memory_manager = NULL;

namespace
{
	int ignore_allocations;
}

MemoryManager::MemoryManager()
{
	// do nothing
	ignore_allocations = false;
}

MemoryManager::~MemoryManager()
{
	ignore_allocations = true;
	_allocations.clear();
}

void MemoryManager::CreateMemoryManager()
{
	DBG_ASSERT(_memory_manager == NULL);

	static MemoryManager internal_mem_manager;
	_memory_manager = &internal_mem_manager;
}

MemoryManager* MemoryManager::GetMemoryManager()
{
	DBG_ASSERT(_memory_manager);

	return _memory_manager;
}

void MemoryManager::AddAllocation(const void* ptr, const unsigned int size, const char* file, const int line)
{
	if (!ignore_allocations)
		_allocations[ptr] = MemoryAllocation(size, file, line);
}

void MemoryManager::RemoveAllocation(const void* ptr)
{
	if (!ignore_allocations)
		_allocations.erase(ptr);
}

void MemoryManager::DebugOutputAllocations()
{
	for (std::map<const void*, MemoryAllocation>::iterator it = _allocations.begin(); it != _allocations.end(); ++it)
	{
		Logging::Log(LOG_Platform, "%s(%d): Memory Leak at 0x%p size: %d bytes\n", it->second._filename, it->second._line_number, it->first, it->second._size);
	}
}
