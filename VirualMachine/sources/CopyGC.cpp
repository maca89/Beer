#include "stdafx.h"
#include "CopyGC.h"
#include "VirtualMachine.h"
#include "Thread.h"

using namespace Beer;


//#define BEER_GC_VERBOSE
//#define BEER_GC_DEBUGGING

#ifdef BEER_GC_VERBOSE
	#define GC_DEBUG(msg) cout << "// CopyGC: " << msg << std::endl;
#else
	#define GC_DEBUG(msg)
#endif



CopyGC::CopyGC(VirtualMachine* vm, size_t memoryLength)
{
	mVM = vm;
	mMemoryOld.init(memoryLength);
	mMemoryNew.init(memoryLength);
	mLiveObjects = 0;
	mLastMoved = 0;
	mLastCollected = 0;
	mCollects = 0;

	// 0 => NULL
	mReferences.push_back(NULL);
	mReferencesNext = 1;
}

CopyGC::~CopyGC()
{
	collectAll();
}

bool CopyGC::enlargeHeap(size_t desiredSize)
{
	cout << "Tried to enlarge heap" << std::endl;
	return false;
}

bool CopyGC::makeSpace(uint32 size)
{
	if(!canAllocate(size)) collect();
	if(!canAllocate(size)) enlargeHeap(size);
	return canAllocate(size);
}

Object* CopyGC::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	//TODO: preOffset
	if(preOffset != 0) throw GCException(BEER_WIDEN("preOffset not yet implemented in CopyGC"));

	uint32 size = roundSize(staticSize + sizeof(Object*) * childrenCount);
	void* data = mMemoryOld.malloc(size);

	if(data == NULL)
	{
		if(!makeSpace(size))
		{
			stringstream ss;
			ss << "Cannot allocate " << size << "bytes";
			throw NotEnoughMemoryException(ss.str());
		}

		data = mMemoryOld.malloc(size);
	}

	Object* obj = reinterpret_cast<Object*>(data);

	obj->setGCFlag(Object::GC_WHITE);
	obj->setTypeFlag(Object::TYPE_REF);
	obj->setClass(NULL);
		
	// children array is at the end of object
	if(childrenCount > 0)
	{
		Object** children = reinterpret_cast<Object**>(reinterpret_cast<byte*>(obj) + staticSize);
		memset(children, 0, childrenCount * sizeof(void*));
		obj->setChildren(children);
	}
#ifdef BEER_GC_DEBUGGING
	else
	{
		obj->setChildren(NULL);
	}
#endif // BEER_GC_DEBUGGING

	mLiveObjects++;
	GC_DEBUG(BEER_WIDEN("Alloced object ") << "#" << obj);
	return obj;
}

void CopyGC::collectAll()
{
	// clear roots
	for(ThreadSet::iterator it = mVM->getThreads().begin(); it != mVM->getThreads().end(); it++)
	{
		Thread* thread = *it;
		WorkStack* stack = thread->getStack();
		stack->clear();
	}
	
	mMemoryOld.clear();
}

void CopyGC::check(MemoryAllocator* memory, Object* object)
{
	if(object == NULL)
	{
		return;
	}

	if(Object::isInlineValue(object))
	{
		return;
	}

	if(!memory->contains(object))
	{
		//memory->contains(object);
		throw GCException(BEER_WIDEN("Object's children not in old space"));
	}

	ClassReflection* klass = mVM->getClass(object);

	if(klass->getChildrenCount(object) > 0 && !memory->contains(object->getChildren()))
	{
		//memory->contains(object->getChildren());
		throw GCException(BEER_WIDEN("Object not in old space"));
	}

	for(uint16 i = 0; i < klass->getChildrenCount(object); i++)
	{
		check(memory, object->getChild(i));
	}
}

void CopyGC::collect()
{
	GC_DEBUG(BEER_WIDEN("Collect"));
	GC_DEBUG(BEER_WIDEN("Used ") << mMemoryOld.getUsed() << " of " << mMemoryOld.getTotal());
	
	mLastMoved = 0;
	mLastCollected = 0;

#ifdef BEER_GC_DEBUGGING
	////////////////////////////////////////////////////////////////////////////////////////
	// debug - check for marked
	////////////////////////////////////////////////////////////////////////////////////////
	for(ThreadSet::iterator it = mVM->getThreads().begin(); it != mVM->getThreads().end(); it++)
	{
		Thread* thread = *it;
		WorkStack* stack = thread->getStack();

		for(uint32 i = 0; i < stack->size(); i++)
		{
			Object* object = stack->top(i);
			check(&mMemoryOld, object);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
#endif // BEER_GC_DEBUGGING

	//StringPool* pool = mVM->getStringClass<StringClass>()->getPool();
	for(ReferenceId i = 1; i < mReferences.size(); i++)
	{
		Object* object = mReferences[i];

		if(object && !Object::isInlineValue(object)) mReferences[i] = move(object);
	}

	// move roots and its children
	for(ThreadSet::iterator it = mVM->getThreads().begin(); it != mVM->getThreads().end(); it++)
	{
		Thread* thread = *it;
		WorkStack* stack = thread->getStack();

		for(uint32 i = 0; i < stack->size(); i++)
		{
			Object* object = stack->top(i);
			if(object && !Object::isInlineValue(object)) stack->set(move(object), i);
		}
	}


#ifdef BEER_GC_DEBUGGING
	std::set<Object*> disposed;

	// call dispose on deleted
	for(Object* object = static_cast<Object*>(mMemoryOld.getFirstNode());
		object != NULL;
		object = static_cast<Object*>(mMemoryOld.getNextNode(object)))
	{
		if(!marked(object))
		{
			if(disposed.find(object) != disposed.end())
			{
				cout << "Disposing second time" << std::endl;
			}

			//object->dispose();
			//mLiveObjects--;
			disposed.insert(object);
			GC_DEBUG(BEER_WIDEN("Disposed object ") << "#" << object << " [" << object->getClass()->getName() << "]");
		}
	}

	disposed.clear();
#endif // BEER_GC_DEBUGGING

	mLastCollected = mLiveObjects - mLastMoved;
	mLiveObjects = mLastMoved;
	mLastMoved = 0;
	mMemoryOld.clear();

	MemoryAllocator::swap(mMemoryNew, mMemoryOld);

#ifdef BEER_GC_DEBUGGING
	for(ThreadSet::iterator it = mVM->getThreads().begin(); it != mVM->getThreads().end(); it++)
	{
		Thread* thread = *it;
		WorkStack* stack = thread->getStack();

		for(uint32 i = 0; i < stack->size(); i++)
		{
			Object* object = stack->top(i);
			check(&mMemoryOld, object);
		}
	}
#endif // BEER_GC_DEBUGGING

	mCollects++;
}

Object* CopyGC::move(Object* object)
{
	DBG_ASSERT(object, BEER_WIDEN("Object is NULL"));
	DBG_ASSERT(!Object::isInlineValue(object), BEER_WIDEN("Object is inline value"));

	if(marked(object)) // already moved
	{
		Object* newObject = reinterpret_cast<Object*>(object->getChildren()); // read saved pointer
		return newObject;
	}

	Object** children = object->getChildren();
	uint32 childrenOffset = reinterpret_cast<uint32>(children) - reinterpret_cast<uint32>(object);
	
	// get object size
	size_t size = mMemoryOld.getNodeSize(object);

	// allocate memory in the other space
	void* data = mMemoryNew.malloc(size);
	
	// cast
	Object* newObject = reinterpret_cast<Object*>(data);

	DBG_ASSERT(newObject != NULL, BEER_WIDEN("Unable to alloc memory in other space"));
	memcpy(newObject, object, size); // copy content
	newObject->setChildren(reinterpret_cast<Object**>(reinterpret_cast<byte*>(static_cast<Object*>(newObject)) + childrenOffset)); // new children pointer
	object->setChildren(reinterpret_cast<Object**>(newObject)); // save new pointer for next generations :-)
	mark(object); // mark object as moved
	//clearMark(newObject); // mark object as moved
	mLastMoved++;

	// move children
	ClassReflection* klass = mVM->getClass(object);
	for(uint32 i = 0; i < klass->getChildrenCount(object); i++)
	{
		Object* child = static_cast<Object*>(children[i]);//object->getChild<Object>(i);
			
		// move and adjust pointer
		if(child && !Object::isInlineValue(child)) newObject->setChild(i, move(child));
	}

#ifdef BEER_GC_DEBUGGING
	check(&mMemoryNew, newObject);
#endif // BEER_GC_DEBUGGING

	GC_DEBUG(BEER_WIDEN("Moved #") << newObject << " [" << static_cast<Object*>(newObject)->getClass()->getName() << "]");

	return newObject;
}