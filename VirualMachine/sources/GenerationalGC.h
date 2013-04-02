#pragma once

#include "prereq.h"
#include "NurseryGC.h"
#include "Frame.h"
#include "Object.h"
#include "StackRef.h"
#include "RememeberdSet.h"
#include "DynamicHeap.h"

namespace Beer
{
	class AllocationBlock;

	typedef DynamicHeap MatureHeap;
	typedef DynamicHeap PermanentHeap;

	class GenerationalGC
	{
	public:

		enum State {
			GC_ALLOC,
			GC_COLLECT
		};

	public:

		typedef uint32 ReferenceId;
		typedef std::vector<Object*> ReferenceVector;

	protected:

		State mState;

		NurseryGC* mNurseryGC;
		
		MatureHeap* mMature;
		PermanentHeap* mPermanent;
		CRITICAL_SECTION mMatureCS;
		CRITICAL_SECTION mPermanentCS;
		ReferenceVector mReferences;
		ReferenceId mReferencesNext;
		RememberedSet mRS;

	public:

		GenerationalGC(size_t nurserySize, size_t blockSize);

		~GenerationalGC();

		INLINE State getState() const
		{
			return mState;
		}

		INLINE Heap* getPermanentHeap()
		{
			return mPermanent;
		}

		void init(VirtualMachine* vm);

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

		/*INLINE void setFrameChild(StackRef<StackFrame> frame, Object* object, int64 index)
		{
		}

		INLINE void getFrameChild(StackRef<StackFrame> frame, int64 index)
		{
		}*/

		INLINE Object* getIdentity(Object* object)
		{
			DBG_ASSERT(object != NULL, BEER_WIDEN("Object is NULL"));

			return GCObject::get(object)->forward();
		}

		INLINE Object* getIdentity(StackRef<Object> object)
		{	
			DBG_ASSERT(*object != NULL, BEER_WIDEN("Object is NULL"));

			return getIdentity(*object);
		}

		INLINE void getChild(StackRef<Object> receiver, StackRef<Object> ret, int64 index)
		{
			DBG_ASSERT(*receiver != NULL, BEER_WIDEN("Object is NULL"));

			ret = getIdentity(receiver)->getChildren()[index];
		}

		// deprecated 
		INLINE void setChild(StackRef<Object> receiver, StackRef<Object> child, int64 index)
		{
			DBG_ASSERT(*receiver != NULL, BEER_WIDEN("Object is NULL"));

			receiver->getChildren()[index] = *child;
		}

		INLINE void setChild(Object* obj, Object* oldChild, Object* newChild)
		{

		}

		// call this when all threads are suspended
		INLINE void threadsSuspended()
		{
			
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
};