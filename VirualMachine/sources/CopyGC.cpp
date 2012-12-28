#include "stdafx.h"
#include "CopyGC.h"
#include "VirtualMachine.h"

using namespace Beer;


//#define BEER_GC_VERBOSE

#ifdef BEER_DEBUG_MODE
	#define BEER_GC_DEBUGGING
#endif

#ifdef BEER_GC_VERBOSE
	#define GC_DEBUG(msg) std::cout << "// CopyGC: " << msg << std::endl;
#else
	#define GC_DEBUG(msg)
#endif



CopyGC::CopyGC(VirtualMachine* vm, WorkStack* stack, size_t memoryLength)
{
	mVM = vm;
	mStack = stack;
	mMemoryOld.init(memoryLength);
	mMemoryNew.init(memoryLength);
	mLiveObjects = 0;
	mLastMoved = 0;
	mLastCollected = 0;
}

CopyGC::~CopyGC()
{
	collectAll();
}

Object* CopyGC::makeGuard(void* data, uint32 size)
{
	// set memory guard
	memset(static_cast<byte*>(data), DbgGuardValue, GuardLength);
	memset(static_cast<byte*>(data) + size - GuardLength, DbgGuardValue, GuardLength);

	// move pointer
	data = static_cast<byte*>(data) + GuardLength;

	// set memory as uninitialised
	memset(data, DbgUninitialisedValue, size - 2*GuardLength);

	// return result
	return reinterpret_cast<Object*>(data);
}

void CopyGC::checkGuard(Object* object)
{
	// TODO
}

bool CopyGC::enlargeHeap(size_t desiredSize)
{
	std::cout << "Tried to enlarge heap" << std::endl;
	return false;
}

bool CopyGC::makeSpace(size_t size)
{
	if(!canAllocate(size)) collect();
	if(!canAllocate(size)) enlargeHeap(size);
	return canAllocate(size);
}

Object* CopyGC::alloc(uint32 staticSize, uint32 childrenCount, int32 preOffset)
{
	//TODO: preOffset
	if(preOffset != 0) throw GCException("preOffset not yet implemented in CopyGC");

	size_t size = roundSize(staticSize + sizeof(Object*) * childrenCount);

#ifdef BEER_GC_DEBUGGING
	size += 2 * GuardLength;
#endif

	if(!makeSpace(size))
	{
		std::stringstream ss;
		ss << "Cannot allocate ";
		ss << size;
		ss << "bytes";
		throw NotEnoughMemoryException(ss.str());
	}

	void* data = mMemoryOld.malloc(size);

	Object* obj
#ifdef BEER_GC_DEBUGGING
		= makeGuard(data, size);
#else
		= reinterpret_cast<Object*>(data);
#endif

	// fix size
#ifdef BEER_GC_DEBUGGING
	size -= 2 * GuardLength;
#endif

	obj->setGCFlag(Object::GC_WHITE);
	obj->setTypeFlag(Object::TYPE_REF);
	obj->setClass(NULL);
	//obj->getHeader().childrenCount = childrenCount;
		
	// children array starts at obj->children
	//obj->children = reinterpret_cast<Object**>(reinterpret_cast<int8*>(obj) + offsetof(Object, children));
		
	// children array is at the end of object
	if(childrenCount > 0)
	{
		Object** children = reinterpret_cast<Object**>(reinterpret_cast<byte*>(obj) + staticSize);
		memset(children, 0, childrenCount * sizeof(void*));
		obj->setChildren(children);
	}
	else
	{
		obj->setChildren(NULL);
	}

	mLiveObjects++;
	GC_DEBUG("Alloced object " << "#" << obj);
	//GC_DEBUG("Used " << mMemoryOld.getUsed() << " of " << mMemoryOld.getTotal());
	return obj;
}

void CopyGC::collectAll()
{
	/*Object* object = static_cast<Object*>(mMemoryOld.getHead());
		
	while(object != NULL)
	{
		object = static_cast<Object*>(mMemoryOld.getNext(object));
	}*/

	mStack->clear(); // clear roots
	mMemoryOld.reset();
}

void CopyGC::check(SimpleMemoryAllocator* memory, Object* object)
{
	if(object == NULL)
	{
		return;
	}

	if(object->isInlineValue())
	{
		return;
	}

	if(!memory->contains(object))
	{
		memory->contains(object);
		throw GCException("Object's children not in old space");
	}

	ClassReflection* klass = mVM->getClassTable()->translate(object);

	if(klass->getChildrenCount(object) > 0 && !memory->contains(object->getChildren()))
	{
		memory->contains(object->getChildren());
		throw GCException("Object not in old space");
	}

	for(uint16 i = 0; i < klass->getChildrenCount(object); i++)
	{
		check(memory, object->getChild(i));
	}
}

void CopyGC::collect()
{
	GC_DEBUG("Collect");
	//GC_DEBUG("Used " << mMemoryOld.getUsed() << " of " << mMemoryOld.getTotal());

	//mVM->printStack();
	
	mLastMoved = 0;
	mLastCollected = 0;

#ifdef BEER_GC_DEBUGGING
	////////////////////////////////////////////////////////////////////////////////////////
	// debug - check for marked
	////////////////////////////////////////////////////////////////////////////////////////
	for(uint32 i = 0; i < mStack->size(); i++)
	{
		Object* object = mStack->top(i);
		check(&mMemoryOld, object);
	}

	mMemoryNew.clear();
	
	for(void* mem = mMemoryOld.getHead();
		mem != NULL;
		mem = mMemoryOld.getNext(mem))
	{
		Object* object = reinterpret_cast<Object*>(
#ifdef BEER_GC_DEBUGGING
			reinterpret_cast<byte*>(mem) + GuardLength
#else
			mem
#endif
		);

		checkGuard(object);

		if(marked(object))
		{
			std::cout << "marked object found when there should be none (heap)" << std::endl;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
#endif // BEER_GC_DEBUGGING

	// move roots and its children
	for(uint32 i = 0; i < mStack->size(); i++)
	{
		Object* object = mStack->top(i);
			
		if(object != NULL)
		{
			mStack->set(move(object), i);
		}
	}


#ifdef BEER_GC_DEBUGGING
	std::set<Object*> disposed;

	// call dispose on deleted
	for(Object* object = static_cast<Object*>(mMemoryOld.getHead());
		object != NULL;
		object = static_cast<Object*>(mMemoryOld.getNext(object)))
	{
		if(!marked(object))
		{
			if(disposed.find(object) != disposed.end())
			{
				std::cout << "Disposing second time" << std::endl;
			}

			//object->dispose();
			//mLiveObjects--;
			disposed.insert(object);
			GC_DEBUG("Disposed object " << "#" << object << " [" << object->getClass()->getName() << "]");
		}
	}

	disposed.clear();
#endif // BEER_GC_DEBUGGING

	mLastCollected = mLiveObjects - mLastMoved;
	mLiveObjects = mLastMoved;
	mLastMoved = 0;

	mMemoryOld.reset();

#ifdef BEER_GC_DEBUGGING
	mMemoryOld.clear(DbgDeletedValue); // DEBUG ONLY: mark memory as deleted
#endif // BEER_GC_DEBUGGING

	SimpleMemoryAllocator::swap(mMemoryNew, mMemoryOld);

	//mVM->printStack();
}

Object* CopyGC::move(Object* object)
{
	if(object == NULL)
	{
		return NULL;
	}

	if(object->isInlineValue())
	{
		return object;
	}

	if(marked(object)) // already moved
	{
		Object* newObject = reinterpret_cast<Object*>(object->getChildren()); // read saved pointer
		//check(&mMemoryNew, object);
		return newObject;
	}

	Object** children = object->getChildren();
	uint32 childrenOffset = reinterpret_cast<uint32>(children) - reinterpret_cast<uint32>(object);
	
	// get object size
	size_t size
#ifdef BEER_GC_DEBUGGING
		= mMemoryOld.getSize(reinterpret_cast<byte*>(object) - GuardLength);// - 2 * GuardLength;
#else
		= mMemoryOld.getSize(object); 
#endif

	// TODO: check guard

	// allocate memory in the other space
	void* data = mMemoryNew.malloc(size);
	
	// cast
	Object* newObject
#ifdef BEER_GC_DEBUGGING
		= makeGuard(data, size); 
#else
		= reinterpret_cast<Object*>(data);
#endif

	// fix size
#ifdef BEER_GC_DEBUGGING
	 size -= 2 * GuardLength;
#endif

	DBG_ASSERT(newObject != NULL, "Unable to alloc memory in other space");
	memcpy(newObject, object, size); // copy content
	newObject->setChildren(reinterpret_cast<Object**>(reinterpret_cast<byte*>(static_cast<Object*>(newObject)) + childrenOffset)); // new children pointer
	object->setChildren(reinterpret_cast<Object**>(newObject)); // save new pointer for next generations :-)
	mark(object); // mark object as moved
	//clearMark(newObject); // mark object as moved
	mLastMoved++;

	// move children
	ClassReflection* klass = mVM->getClassTable()->translate(object);
	for(uint32 i = 0; i < klass->getChildrenCount(object); i++)
	{
		Object* child = static_cast<Object*>(children[i]);//object->getChild<Object>(i);
			
		// move and adjust pointer
		newObject->setChild(i, move(child));
	}

#ifdef BEER_GC_DEBUGGING
	check(&mMemoryNew, newObject);
#endif // BEER_GC_DEBUGGING

	GC_DEBUG("Moved #" << newObject << " [" << static_cast<Object*>(newObject)->getClass()->getName() << "]");

	return newObject;
}