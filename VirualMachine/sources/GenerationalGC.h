#pragma once

#include "prereq.h"
#include "NurseryGC.h"
#include "StackFrame.h"
#include "Object.h"

namespace Beer
{
	class AllocationBlock;
	class Heap;
	class SimpleMemoryAllocator;

	class GenerationalGC
	{
	public:

		enum State {
			Inactive,
			Collect
		};

	public:

		typedef uint32 ReferenceId;
		typedef std::vector<Object*> ReferenceVector;

	protected:

		State mState;

		NurseryGC * mNurseryGC;
		
		SimpleMemoryAllocator * mMature;
		SimpleMemoryAllocator * mPermanent;
		CRITICAL_SECTION mMatureCS;
		CRITICAL_SECTION mPermanentCS;
		ReferenceVector mReferences;
		ReferenceId mReferencesNext;

	public:

		GenerationalGC(unsigned char nurseryBitSize, size_t blockSize);

		~GenerationalGC();

		INLINE State getState() const
		{
			return mState;
		}

		void init();

		INLINE Heap* createHeap()
		{
			return mNurseryGC->createHeap();
		}

		Object* alloc(uint32 staticSize, uint32 childrenCount = 0, int32 preOffset = 0);

		void collect();

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

		INLINE Object* getChild(Object* parent, int64 index)
		{
			return parent->mChildren[index];
		}

		INLINE void pushChild(StackFrame* stack, int64 index)
		{
			Object* obj = stack->stackTop();
			stack->stackPop();
			stack->stackPush(obj->mChildren[index]);
		}

		INLINE void setChild(StackFrame* stack, int64 index)
		{
			Object* child = stack->stackTop();
			stack->stackPop();
			Object* parent = stack->stackTop();
			stack->stackPop();
			parent->mChildren[index] = child; 
		}
	};

	template <class T>
	struct Reference
	{
	public:
		typedef GenerationalGC::ReferenceId Id;

	protected:
		GenerationalGC * mGC; // TODO: get rid of
		Id mId;

	public:
		INLINE Reference() : mGC(NULL), mId(NULL)
		{
		}

		INLINE Reference(GenerationalGC* gc, Id id) : mId(id), mGC(gc)
		{
		}

		INLINE Reference(GenerationalGC* gc, Object* object) : mGC(gc)
		{
			mId = mGC->reference(object);
		}

		INLINE ~Reference()
		{
		}

		INLINE T* translate(GenerationalGC* gc) { return gc->translate<T>(this->mId); }

		INLINE operator bool() const {  return mId == 0; }

		INLINE Id getId() const { return mId; }

		INLINE T* get() { return mGC->translate<T>(this->mId); }
		INLINE const T* get() const { return mGC->translate<T>(this->mId); }

		INLINE T* operator->() { return get(); }
		INLINE const T* operator->() const { return get(); }

		INLINE T* operator* () { return get(); }
		INLINE const T* operator* () const { return get(); }
	};
}