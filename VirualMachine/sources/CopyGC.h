#pragma once
#include "prereq.h"
#include "GarbageCollector.h"
#include "SimpleMemoryAllocator.h"
#include "Object.h"
#include "StackFrame.h"


namespace Beer
{
	class VirtualMachine;

	class CopyGC : public GarbageCollector
	{
	public:
		typedef uint32 ReferenceId;
		typedef std::vector<Object*> ReferenceVector;

	protected:
		SimpleMemoryAllocator mMemoryOld;
		SimpleMemoryAllocator mMemoryNew;
		uint32 mLiveObjects;
		uint32 mLastMoved;
		uint32 mLastCollected;
		WorkStack* mStack;
		VirtualMachine* mVM;
		ReferenceVector mReferences;
		ReferenceId mReferencesNext;

		static const int GuardLength = 256;
		static const int DbgGuardValue = 0xfdfdfdfd;
		static const int DbgUninitialisedValue = 0xcdcdcdcd;
		static const int DbgDeletedValue = 0xfeeefeee;

	public:
		CopyGC(VirtualMachine* vm, WorkStack* stack, size_t memoryLength = 1024*1); // 1KB
		virtual ~CopyGC();

		INLINE uint32 countLiveObjects()
		{
			return mLiveObjects;
		}

		INLINE uint32 countLastCollected()
		{
			return mLastCollected;
		}

		// GarbageCollector
		virtual Object* alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset = 0);
		virtual void collect();

		INLINE Object* translate(const ReferenceId& id)
		{
			DBG_ASSERT(id < mReferences.size(), BEER_WIDEN("Tried to reference an unreferenced object"));
			return mReferences[id];
		}
		
		template <typename T>
		INLINE T* translate(const ReferenceId& ref)
		{
			return static_cast<T*>(translate(ref));
		}

		NOINLINE ReferenceId reference(Object* object)
		{
			for(; mReferencesNext < mReferences.size(); mReferencesNext++)
			{
				if(mReferences[mReferencesNext] == NULL)
				{
					mReferences[mReferencesNext] = object;
					return mReferencesNext++;
				}
			}

			mReferences.push_back(object);
			return mReferencesNext++;
		}

	protected:
		bool makeSpace(size_t size);
		bool enlargeHeap(size_t desiredSize = 0);
		void collectAll();
		Object* move(Object* object);
		void check(SimpleMemoryAllocator* memory, Object* object);

		INLINE size_t roundSize(size_t size)
		{
			return size + (size & 1); // rounds size to the closest bigger even number
		}
	
		INLINE bool canAllocate(size_t size) const
		{
			return mMemoryOld.canMalloc(size);
		}

		INLINE bool marked(Object* object)
		{
			return object->getGCFlag() == Object::GC_BLACK;
		}

		INLINE void mark(Object* object)
		{
			object->setGCFlag(Object::GC_BLACK);
		}

		INLINE void clearMark(Object* object)
		{
			object->setGCFlag(Object::GC_WHITE);
		}

		Object* makeGuard(void* data, uint32 size);
		void checkGuard(Object* object);
	};

	template <class T>
	struct Reference
	{
	public:
		typedef CopyGC::ReferenceId Id;

	protected:
		CopyGC* mHeap; // TODO: get rid of
		Id mId;

	public:
		INLINE Reference() : mHeap(NULL), mId(NULL)
		{
		}

		INLINE Reference(CopyGC* heap, Id id) : mId(id), mHeap(heap)
		{
		}

		INLINE Reference(CopyGC* heap, Object* object) : mHeap(heap)
		{
			mId = mHeap->reference(object);
		}

		INLINE ~Reference()
		{
		}

		INLINE T* translate(CopyGC* gc) { return gc->translate<T>(this->mId); }

		INLINE operator bool() const {  return mId == 0; }

		INLINE Id getId() const { return mId; }

		INLINE T* get() { return mHeap->translate<T>(this->mId); }
		INLINE const T* get() const { return mHeap->translate<T>(this->mId); }

		INLINE T* operator->() { return get(); }
		INLINE const T* operator->() const { return get(); }

		INLINE T* operator* () { return get(); }
		INLINE const T* operator* () const { return get(); }
	};
};