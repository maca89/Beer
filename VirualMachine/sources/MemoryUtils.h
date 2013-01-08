#pragma once
#include "prereq.h"


/*namespace Beer
{
	class MemoryUtils
	{
		enum MemoryMode
		{
			// windows constants: http://msdn.microsoft.com/en-us/library/windows/desktop/aa366786(v=vs.85).aspx
			MODE_EXECUTE = PAGE_EXECUTE,
			MODE_EXECUTE_READ = PAGE_EXECUTE_READ,
			MODE_EXECUTE_READWRITE = PAGE_EXECUTE_READWRITE,
			MODE_EXECUTE_WRITECOPY  = PAGE_EXECUTE_WRITECOPY,
			MODE_NOACCESS = PAGE_NOACCESS,
			MODE_READONLY = PAGE_READONLY,
			MODE_READWRITE = PAGE_READWRITE,
			MODE_WRITECOPY = PAGE_WRITECOPY,
			MODE_GUARD = PAGE_GUARD,
			MODE_NOCACHE = PAGE_NOCACHE,
			MODE_WRITECOMBINE = PAGE_WRITECOMBINE,

			MODE_STACK_GUARD = PAGE_GUARD | PAGE_READONLY,
			MODE_HEAP_GUARD = PAGE_NOACCESS,
		};

		static MemoryMode getMode(void* mem);
		static void setMode(void* mem, uint32 length, MemoryMode mode);
	};
};*/