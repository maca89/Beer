#pragma once
#include "prereq.h"


namespace Beer
{
	class GrowableMemoryAllocator
	{
	public:
		typedef uint32 length_t;
		//typedef uint64 length_t;

		#pragma pack(push, 1)
		struct BlockHead
		{
			length_t commitedPages;
			length_t reservedPages;
			//void* previous;
			//void* start;
			BlockHead* next;
		};
		#pragma pack(pop)

		#pragma pack(push, 1)
		struct BlockGuard
		{
			uint32 checksum;
			BlockHead* head;
		};
		#pragma pack(pop)

		static const int DbgUninitialisedValue = 0xcdcdcdcd;
		static const int MultiplesForHeadStore = 512;

	protected:
		BlockHead* mLast;

	public:
		INLINE GrowableMemoryAllocator() : mLast(NULL) { }
		INLINE ~GrowableMemoryAllocator() { destroy(); }

		NOINLINE void* malloc(length_t commitSize, length_t reserveSize) // commitSize <= reserveSize
		{
			DBG_ASSERT(commitSize <= reserveSize, BEER_WIDEN("Cannot commit more memory than reserved"));

			length_t pages = howMuchPages(reserveSize + sizeof(BlockHead));
			length_t blockSize = pages * gPageSize;
			void* block = VirtualAlloc(NULL, pages * gPageSize, MEM_RESERVE, PAGE_READWRITE);
#ifdef BEER_DEBUG
			if(!block)
			{
				PrintLastError();
				DBG_ASSERT(false, BEER_WIDEN("Unable to alloc more memory"));
				return NULL;
			}
#endif // BEER_DEBUG

			// commit the first page
			block = VirtualAlloc(block, gPageSize, MEM_COMMIT, PAGE_READWRITE);
#ifdef BEER_DEBUG
			if(!block)
			{
				PrintLastError();
				VirtualFree(block, pages * gPageSize, MEM_RELEASE);
				DBG_ASSERT(false, BEER_WIDEN("Unable to commit head page"));
				return NULL;
			}
#endif // BEER_DEBUG

			BlockHead* head = reinterpret_cast<BlockHead*>(reinterpret_cast<byte*>(block));
			head->next = NULL;
			head->commitedPages = 1;
			head->reservedPages = pages;

			// create a guard
			if(pages > 1)
			{
				// set guard pointer
				BlockGuard* guard = reinterpret_cast<BlockGuard*>(reinterpret_cast<byte*>(block) + gPageSize);
			
				// save head pointer all over the guard
				bool result = initGuardPage(guard, head);
#ifdef BEER_DEBUG
				if(!result)
				{
					VirtualFree(block, pages * gPageSize, MEM_RELEASE);
					DBG_ASSERT(false, BEER_WIDEN("Unable to create guard"));
					return NULL;
				}
#endif // BEER_DEBUG
			}

			if(mLast)
			{
				mLast->next = head;
				mLast = head;
			}
			else
			{
				mLast = head;
			}

			return reinterpret_cast<byte*>(head) + sizeof(BlockHead);
		}

		INLINE void free(void* block)
		{
			// TODO
		}

	protected:
		struct StaticInitializer;
		friend struct StaticInitializer;

		static StaticInitializer gStaticInitializer;
		static PVOID gExceptionHandlerPtr;
		static length_t gPageSize;
		
		static LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS exceptionInfo)
		{
			if(exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
			{
				void* addr = reinterpret_cast<void*>(exceptionInfo->ExceptionRecord->ExceptionInformation[1]);
				BlockGuard* guard = reinterpret_cast<BlockGuard*>(reinterpret_cast<byte*>(addr) + reinterpret_cast<length_t>(addr) % MultiplesForHeadStore);

				if(countCheckSum(guard) != guard->checksum)
				{
#ifdef BEER_DEBUG
					cout << "[Not a valid BlockGuard]\n";
#endif // BEER_DEBUG
					return EXCEPTION_CONTINUE_SEARCH;
				}

				BlockHead* head = guard->head;

				if(head->commitedPages < head->reservedPages)
				{
					BlockGuard* nextGuard = reinterpret_cast<BlockGuard*>(reinterpret_cast<byte*>(head) + head->commitedPages * gPageSize);

					// save head pointer all over the guard to allow more growing
					bool result = initGuardPage(nextGuard, head);
#ifdef BEER_DEBUG
					if(!result)
					{
						DBG_ASSERT(false, BEER_WIDEN("Unable to create guard"));
						return EXCEPTION_CONTINUE_SEARCH;
					}
#endif // BEER_DEBUG

					// update pages count
					head->commitedPages++;

#ifdef BEER_DEBUG
					cout << "[Guarded memory growed]\n";
#endif // BEER_DEBUG
				}

				// protection of the page is automatically reset to READ/WRITE,
				// next attempt to access same memory won't fail
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			// unknown exception
			return EXCEPTION_CONTINUE_SEARCH;
		}

		INLINE void destroy()
		{
			// TODO: free all allocated nodes
			//VirtualFree(mHead, mSize, MEM_RELEASE);
			//mSize = 0;
		}

		INLINE length_t howMuchPages(length_t memory) const
		{
			return (memory / gPageSize) + ((memory % gPageSize) == 0 ? 0 : 1);
		}

		INLINE static uint32 countCheckSum(const BlockGuard* guard)
		{
			return reinterpret_cast<uint32>(guard) ^ reinterpret_cast<uint32>(guard->head);
		}

		NOINLINE static bool initGuardPage(BlockGuard* guard, BlockHead* head)
		{
			void* result1 = NULL;
			DWORD result2 = 0;

			// commit
			result1 = VirtualAlloc(guard, gPageSize, MEM_COMMIT, PAGE_READWRITE);
	#ifdef BEER_DEBUG
			if(!result1)
			{
				PrintLastError();
				DBG_ASSERT(false, BEER_WIDEN("Unable to commit guard page"));
				return false;
			}
	#endif // BEER_DEBUG

			// save pointer to head all over the page
			for(length_t offset = reinterpret_cast<length_t>(guard) % MultiplesForHeadStore; (offset + sizeof(BlockHead*)) < gPageSize; offset += MultiplesForHeadStore)
			{
				BlockGuard* realGuard = reinterpret_cast<BlockGuard*>(reinterpret_cast<byte*>(guard) + offset);
				realGuard->head = head;
				realGuard->checksum = countCheckSum(realGuard);
			}

			// mark as guard
			result2 = VirtualProtect(guard, gPageSize, PAGE_READWRITE | PAGE_GUARD, &result2);
#ifdef BEER_DEBUG
			if(result2 == 0)
			{
				PrintLastError();
				DBG_ASSERT(false, BEER_WIDEN("Unable to protect guard page"));
				return false;
			}
#endif // BEER_DEBUG

			return true;
		}

		static void PrintLastError()
		{
			DWORD errCode = GetLastError();

			LPWSTR err;
			if(!FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
				(LPTSTR) &err,
				0,
				NULL))
			{
				err = BEER_WIDEN("Unable to obtain error message\n");
			}

			cout << err;

			LocalFree(err);
		}
	};
};

/*DWORD oldProtect = 0;
if(VirtualProtect(reinterpret_cast<byte*>(nextGuard), 4, PAGE_READWRITE | PAGE_GUARD, &oldProtect) == 0)
{
	cout << "[Unable to call VirtualProtect]\n";
}
else
{
	DWORD oldProtectCheck = 0;
	cout << "[" << oldProtect << "|";
	if((oldProtect & PAGE_EXECUTE) == PAGE_EXECUTE) { cout << "PAGE_EXECUTE|"; oldProtectCheck |= PAGE_EXECUTE; }
	if((oldProtect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ) { cout << "PAGE_EXECUTE_READ|"; oldProtectCheck |= PAGE_EXECUTE_READ; }
	if((oldProtect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE) { cout << "PAGE_EXECUTE_READWRITE|"; oldProtectCheck |= PAGE_EXECUTE_READWRITE; }
	if((oldProtect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY) { cout << "PAGE_EXECUTE_WRITECOPY|"; oldProtectCheck |= PAGE_EXECUTE_WRITECOPY; }
	if((oldProtect & PAGE_GUARD) == PAGE_GUARD) { cout << "PAGE_GUARD|"; oldProtectCheck |= PAGE_GUARD; }
	if((oldProtect & PAGE_NOACCESS) == PAGE_NOACCESS) { cout << "PAGE_NOACCESS|"; oldProtectCheck |= PAGE_NOACCESS; }
	if((oldProtect & PAGE_NOCACHE) == PAGE_NOCACHE) { cout << "PAGE_NOCACHE|"; oldProtectCheck |= PAGE_NOCACHE; }
	if((oldProtect & PAGE_READONLY) == PAGE_READONLY) { cout << "PAGE_READONLY|"; oldProtectCheck |= PAGE_READONLY; }
	if((oldProtect & PAGE_READWRITE) == PAGE_READWRITE) { cout << "PAGE_READWRITE|"; oldProtectCheck |= PAGE_READWRITE; }
	if((oldProtect & PAGE_REVERT_TO_FILE_MAP) == PAGE_REVERT_TO_FILE_MAP) { cout << "PAGE_REVERT_TO_FILE_MAP|"; oldProtectCheck |= PAGE_REVERT_TO_FILE_MAP; }
	if((oldProtect & PAGE_WRITECOMBINE) == PAGE_WRITECOMBINE) { cout << "PAGE_WRITECOMBINE|"; oldProtectCheck |= PAGE_WRITECOMBINE; }
	if((oldProtect & PAGE_WRITECOPY) == PAGE_WRITECOPY) { cout << "PAGE_WRITECOPY|"; oldProtectCheck |= PAGE_WRITECOPY; }
	cout << "==" << oldProtectCheck << "]\n";
}*/