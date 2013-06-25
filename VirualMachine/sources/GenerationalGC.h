#pragma once

#include "prereq.h"
#include "NurseryGC.h"
#include "MatureGC.h"
#include "Frame.h"
#include "Object.h"
#include "StackRef.h"
#include "RememeberdSet.h"
#include "DynamicHeap.h"
#include "FixedHeap.h"
#include "Mutex.h"


#define BEER_GC_STATS

namespace Beer
{
	class GCObject;
	class VirtualMachine;
	class AllocationBlock;

	typedef FixedHeap MatureHeap;
	typedef FixedHeap PermanentHeap;	


	class GenerationalGC
	{
	public:

		enum State {
			INACTIVE,
			NURSERY_COLL_PENDING,
			NURSERY_COLL_RUNNING,
			MATURE_COLL_PENDING,
			MATURE_COLL_RUNNING
		};

		enum Generation {
			GEN_NURSERY,
			GEN_MATURE
		};

		

#ifdef BEER_GC_STATS

		struct Statistics
		{
			uint32 mNurseryCollections;
			uint32 mMatureCollections;
			uint32 mPromoted;
			uint32 mAllocatedNursery;
			uint32 mAllocatedMature;


			Statistics()
				:	mNurseryCollections(0),
					mMatureCollections(0),
					mPromoted(0),
					mAllocatedNursery(0),
					mAllocatedMature(0)
			{ }

		} mStats;

#endif
		
	public:

		typedef uint32 ReferenceId;
		typedef std::vector<Object*> ReferenceVector;

	protected:

		State mState;

		VirtualMachine* mVM;

		NurseryGC* mNurseryGC;
		MatureGC* mMatureGC;

		byte* mMemPermStart;
		PermanentHeap* mPermanent;

		ReferenceVector mReferences;
		ReferenceId mReferencesNext;

		RememberedSet* mRemSet;

		CriticalSection mStateMutex;

	public:

		GenerationalGC(uint32 nurserySize, uint32 blockSize, uint32 matureSize, uint32 permanentSize);

		~GenerationalGC();

		INLINE State getState() const
		{
			return mState;
		}

		INLINE MatureGC* getMatureGC()
		{
			return mMatureGC;
		}

		INLINE Heap* getPermanentHeap()
		{
			return mPermanent;
		}

		void init(VirtualMachine* vm);

		INLINE virtual Heap* createHeap()
		{
			return mNurseryGC->createHeap();
		}

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


		INLINE Object* getIdentity(Object* object)
		{
			DBG_ASSERT(object != NULL, BEER_WIDEN("Object is NULL"));

			if(Object::isInlineValue(object)) return object;
			return GCObject::get(object)->forward();
		}

		INLINE Object* getIdentity(StackRef<Object> object)
		{	
			DBG_ASSERT(*object != NULL, BEER_WIDEN("Object is NULL"));

			return getIdentity(*object);
		}

		NOINLINE void getChild(StackRef<Object> receiver, StackRef<Object> ret, int64 index)
		{
			DBG_ASSERT(*receiver != NULL, BEER_WIDEN("Object is NULL"));

#ifdef BEER_DEBUG_MODE
			{
				GCObject* gcObj = GCObject::get(*receiver);

				byte* start = reinterpret_cast<byte*>(gcObj) + sizeof(GCObject);
				byte* end = start + gcObj->getSize() - sizeof(GCObject);

				byte* child = reinterpret_cast<byte*>(receiver->getChildren()) + index * sizeof(Object*);

				if (start < child && end < child)
				{
					BEER_BREAKPOINT();
					throw GCException(BEER_WIDEN("Accessing child outside of an object"));
				}
			}
#endif
			ret = getIdentity(receiver)->getChildren()[index];
		}

		// deprecated 
		NOINLINE void setChild(StackRef<Object> receiver, StackRef<Object> child, int64 index)
		{
			DBG_ASSERT(*receiver != NULL, BEER_WIDEN("Object is NULL"));

#ifdef BEER_DEBUG_MODE
			{
				GCObject* gcObj = GCObject::get(*receiver);

				byte* start = reinterpret_cast<byte*>(gcObj) + sizeof(GCObject);
				byte* end = start + gcObj->getSize() - sizeof(GCObject);

				byte* child = reinterpret_cast<byte*>(receiver->getChildren()) + index * sizeof(Object*);

				if (start < child && end < child)
				{
					throw GCException(BEER_WIDEN("Storing child outside of an object"));
				}
			}
#endif

			if (mMatureGC->contains(reinterpret_cast<byte*>(*receiver)))
			{
				Object** ref = &receiver->getChildren()[index];

				if (mNurseryGC->contains(*ref))
				{
					mRemSet->remove(GCObject::get(*ref), ref);
				}

				if (mNurseryGC->contains(child.get()))
				{
					mRemSet->add(GCObject::get(*child), ref, GCObject::get(*receiver));
				}
			}

			receiver->getChildren()[index] = *child;
		}

		NOINLINE void setChild(Object* obj, Object* oldChild, Object* newChild)
		{
			
		}

		void startCollection(Generation gen);
		void stopCollection(Generation gen);

		void threadsSuspended();

		void afterCollection();
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