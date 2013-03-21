#pragma once
#include "prereq.h"
#include "Object.h"
#include "Frame.h"
#include "Pair.h"
#include "Property.h"
#include "String.h" // TODO
#include "TraverseObjectReceiver.h"


namespace Beer
{
	class Method;
	class Frame;
	class VirtualMachine;
	class Thread;

	class Class : public Object
	{
	public:
		enum
		{
			FLAG_VALUE_TYPE = 0x01,
			FLAG_INTERFACE = 0x02
		};

		typedef void (*Traverser)(TraverseObjectReceiver* receiver, Class* klass, Object* instance);
		//typedef void (*TraverseClassFn)(TraverseObjectReceiver* receiver, Class* klass);

		enum
		{
			// children order (not including those of Object parent class):
			// #1 name
			// #2 parents[]
			// #3 methods[]
			// #4 properties[]
			CLASS_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 1,

			CHILD_ID_CLASS_NAME = OBJECT_CHILDREN_COUNT,
		};

		friend class MetaClass;

		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint8 mFlags;

		uint32 mParentsCount;
		uint32 mPropertiesCount;
		uint32 mMethodsCount;
		
		uint32 mParentNext;
		uint32 mPropertyNext;
		uint32 mMethodNext;

		Traverser mTraverser;
		////////////////////////////////////////////////////////////

	public:

		// flags
		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }
		
		INLINE void markAsValueType() { markFlag(FLAG_VALUE_TYPE); }
		INLINE bool isValueType() const { return hasFlag(FLAG_VALUE_TYPE); }
		
		INLINE void markAsInterface() { markFlag(FLAG_INTERFACE); }
		INLINE bool isInterface() const { return hasFlag(FLAG_INTERFACE); }

		INLINE bool hasFlag(uint8 n) const { return (mFlags & n) == n; }
		INLINE void markFlag(uint8 n) { mFlags |= n; }

		// traverser

		Traverser getTraverser() const;
		void setTraverser(Traverser value);

		// parents

		uint32 getParentsCount() const;
		uint32 getMethodsCount() const;
		uint32 getPropertiesCount() const;

		INLINE String* getName() { return static_cast<String*>(getChildren()[CHILD_ID_CLASS_NAME]); }

		// methods

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret);

		static void BEER_CALL findMethod(Thread* thread, StackRef<Class> receiver, StackRef<String> selector, StackRef<Method> ret);
		static void BEER_CALL findMethodIndex(Thread* thread, StackRef<Class> receiver, StackRef<String> selector, StackRef<Method> ret1, StackRef<Integer> ret2);
		static void BEER_CALL substituable(Thread* thread, StackRef<Class> receiver, StackRef<Class> otherClass, StackRef<Boolean> ret);
		
		static void BEER_CALL getName(Thread* thread, StackRef<Class> receiver, StackRef<String> ret);
		static void BEER_CALL setName(Thread* thread, StackRef<Class> receiver, StackRef<String> value);
		
		static void BEER_CALL getParent(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Class> ret);
		static void BEER_CALL getParentsCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL addParent(Thread* thread, StackRef<Class> receiver, StackRef<Class> value);

		static void BEER_CALL getProperty(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Property> ret);
		static void BEER_CALL getPropertiesCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL addProperty(Thread* thread, StackRef<Class> receiver, StackRef<Property> value);
		
		static void BEER_CALL getMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Pair> ret);
		static void BEER_CALL getMethodsCount(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL addMethod(Thread* thread, StackRef<Class> receiver, StackRef<Pair> value);
		
		// shortcut
		//static void BEER_CALL getOnlyMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Method> ret);
		static void getProperty(Thread* thread, StackRef<Class> receiver, uint32 index, StackRef<Property> ret);

		// traversers
		static void DefaultInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance);

	protected:
		static void BEER_CALL getPropertyNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL getMethodNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL getParentNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);

		static void BEER_CALL incrPropertyNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL incrMethodNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL incrParentNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);

		static bool hasPropertyFreeSlot(Thread* thread, StackRef<Class> receiver);
		static bool hasMethodFreeSlot(Thread* thread, StackRef<Class> receiver);
		static bool hasParentFreeSlot(Thread* thread, StackRef<Class> receiver);
	};

	// inline definitions

	INLINE Class::Traverser Class::getTraverser() const
	{
		return mTraverser;
	}

	INLINE void Class::setTraverser(Class::Traverser value)
	{
		mTraverser = value;
	}

	INLINE uint32 Class::getParentsCount() const
	{
		return mParentsCount;
	}
	
	INLINE uint32 Class::getMethodsCount() const
	{
		return mMethodsCount;
	}
	
	INLINE uint32 Class::getPropertiesCount() const
	{
		return mPropertiesCount;
	}
};