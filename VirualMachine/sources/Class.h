#pragma once
#include "prereq.h"
#include "Object.h"
#include "Selector.h"
#include "Pair.h"
#include "Property.h"
#include "String.h" // TODO


namespace Beer
{
	class EventReflection;
	class Method;
	struct GarbageCollector;
	class StackFrame;
	class VirtualMachine;

	//#pragma pack(push, 1)
	class Class : public Object
	{
	public:
		enum
		{
			FLAG_VALUE_TYPE = 0x01,
			FLAG_INTERFACE = 0x02
		};

		enum
		{
			// children order (not including those of Object parent class):
			// #1 name
			// #2 parents[] // TODO: size
			// #3 methods[] // TODO: size
			// #4 properties[] // TODO: size
			CLASS_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 1,
		};

		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint8 mFlags;

		// TODO: garbaged
		uint16 mParentsCount;
		uint32 mPropertiesCount;
		uint16 mMethodsCount;
		//uint32 mDefaultChildrenCount;
		////////////////////////////////////////////////////////////

	public:
		INLINE Class()
		{
		}

		// flags
		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }
		
		INLINE void markAsValueType() { markFlag(FLAG_VALUE_TYPE); }
		INLINE bool isValueType() const { return hasFlag(FLAG_VALUE_TYPE); }
		
		INLINE void markAsInterface() { markFlag(FLAG_INTERFACE); }
		INLINE bool isInterface() const { return hasFlag(FLAG_INTERFACE); }

		INLINE bool hasFlag(uint8 n) const { return (mFlags & n) == n; }
		INLINE void markFlag(uint8 n) { mFlags |= n; }

		// children count, TODO: get rid of
		//INLINE void setDefaultChildrenCount(uint32 value) { mDefaultChildrenCount = value; }
		//INLINE uint32 getDefaultChildrenCount() { return mDefaultChildrenCount; }

		//virtual uint64 getChildrenCount(Object* object) { return mDefaultChildrenCount; }
		
		// name

		INLINE String* getName() const
		{
			return getChild<String>(OBJECT_CHILDREN_COUNT);
		}
		
		INLINE void setName(String* value)
		{
			setChild(OBJECT_CHILDREN_COUNT, value);
		}

		// parents

		INLINE uint16 getParentsCount() const
		{
			return mParentsCount;
		}

		INLINE void setParent(uint16 i, Class* parent)
		{
			DBG_ASSERT(i < getParentsCount(), BEER_WIDEN("Unable to add more parents"));
			setChild(OBJECT_CHILDREN_COUNT + 1 + i, parent); // 1 for name
		}
		
		INLINE Class* getParent(uint16 i) const
		{
			DBG_ASSERT(i < getParentsCount(), BEER_WIDEN("Unknown parent"));
			return getChild<Class>(OBJECT_CHILDREN_COUNT + 1 + i); // 1 for name
		}

		// mehods

		INLINE uint16 getMethodsCount() const
		{
			return mMethodsCount;
		}

		INLINE void setMethod(uint16 i, Pair* method)
		{
			DBG_ASSERT(i < getMethodsCount(), BEER_WIDEN("Unable to add more methods"));
			setChild(OBJECT_CHILDREN_COUNT + 1 + getParentsCount() + i, method); // 1 for name
		}
		
		INLINE Pair* getMethod(uint16 i) const
		{
			DBG_ASSERT(i < getMethodsCount(), BEER_WIDEN("Unknown method"));
			return getChild<Pair>(OBJECT_CHILDREN_COUNT + 1 + getParentsCount() + i); // 1 for name
		}

		// properties

		INLINE uint32 getPropertiesCount() const
		{
			return mPropertiesCount;
		}

		INLINE void setProperty(uint32 i, Property* prop)
		{
			DBG_ASSERT(i < getPropertiesCount(), BEER_WIDEN("Unable to add more properties")); 
			setChild(OBJECT_CHILDREN_COUNT + 1 + getParentsCount() + getMethodsCount() + i, prop); // 1 for name
		}

		INLINE Property* getProperty(uint32 i) const
		{
			DBG_ASSERT(i < getPropertiesCount(), BEER_WIDEN("Unknown property"));
			return getChild<Property>(OBJECT_CHILDREN_COUNT + 1 + getParentsCount() + getMethodsCount() + i); // 1 for name
		}

		// 

		void extends(uint16 i, Class* klass);
		Method* findMethod(String* selector);
		bool substituable(Class* otherClass) const;

		// instance creation

		// TODO: get rid of virtual methods
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc) = 0 { return NULL; }

		template <typename T>
		INLINE T* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc) { return static_cast<T*>(createInstance(vm,  frame, gc));}

	protected: // TODO: private
		INLINE ~Class()
		{
			// never called!
		}
	};
	//#pragma pack(pop)
};