#pragma once
#include "prereq.h"
#include "Object.h"
#include "Frame.h"
#include "Pair.h"
#include "String.h" // TODO
#include "TraverseObjectReceiver.h"
#include "Property.h"


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
			FLAG_INTERFACE = 0x02,
			FLAG_ABSTRACT = 0x04,//TODO
			FLAG_SEALED = 0x08,//TODO

			FLAG_ENTRYPOINT = 0x10,
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
		
		INLINE void markEntryPoint() { markFlag(FLAG_ENTRYPOINT); }
		INLINE bool isEntryPoint() const { return hasFlag(FLAG_ENTRYPOINT); }
		
		INLINE void markAbstract() { markFlag(FLAG_ABSTRACT); }
		INLINE bool isAbstract() const { return hasFlag(FLAG_ABSTRACT); }
		
		INLINE void markSealed() { markFlag(FLAG_SEALED); }
		INLINE bool isSealed() const { return hasFlag(FLAG_SEALED); }

		INLINE bool hasFlag(uint8 n) const { return (mFlags & n) == n; }
		INLINE void markFlag(uint8 n) { mFlags |= n; }

		// traverser

		Traverser getTraverser() const;
		void setTraverser(Traverser value);

		// children

		bool hasPropertyFreeSlot() const;
		bool hasMethodFreeSlot() const;
		bool hasParentFreeSlot() const;

		String* getName();
		void setName(String* value);
		
		Class* getParent(uint32 index);
		uint32 getParentsCount() const;
		uint32 getParentNext() const;
		void addParent(Class* value);
		void setParent(uint32 index, Class* value);
		
		Property* getProperty(uint32 index);
		uint32 getPropertiesCount() const;
		uint32 getPropertyNext() const;
		void addProperty(Property* value);
		void setProperty(uint32 index, Property* value);
		
		Pair* getMethod(uint32 index);
		uint32 getMethodsCount() const;
		uint32 getMethodNext() const;
		void addMethod(Pair* value);
		void setMethod(uint32 index, Pair* value);

		//bool substituable(Class otherClass); // TODO

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
		static void BEER_CALL incrPropertyNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL incrMethodNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
		static void BEER_CALL incrParentNext(Thread* thread, StackRef<Class> receiver, StackRef<Integer> ret);
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

	// slots

	INLINE bool Class::hasPropertyFreeSlot() const
	{
		return mPropertyNext < mPropertiesCount;
	}

	INLINE bool Class::hasMethodFreeSlot() const
	{
		return mMethodNext < mMethodsCount;
	}

	INLINE bool Class::hasParentFreeSlot() const
	{
		return mParentNext < mParentsCount;
	}

	// parents

	INLINE Class* Class::getParent(uint32 index)
	{
		BOUNDS_ASSERT(index, mParentsCount);
		return static_cast<Class*>(getChild(CHILD_ID_CLASS_NAME + 1 + index)); // +1 for name 
	}

	INLINE void Class::setParent(uint32 index, Class* value)
	{
		BOUNDS_ASSERT(index, mParentsCount);
		setChild(CHILD_ID_CLASS_NAME + 1 + index, value); // +1 for name 
	}

	INLINE uint32 Class::getParentNext() const
	{
		return mParentNext;
	}

	INLINE uint32 Class::getParentsCount() const
	{
		return mParentsCount;
	}

	// methods

	INLINE Pair* Class::getMethod(uint32 index)
	{
		BOUNDS_ASSERT(index, mMethodsCount);
		return static_cast<Pair*>(getChild(CHILD_ID_CLASS_NAME + 1 + mParentsCount + index)); // +1 for name 
	}

	INLINE void Class::setMethod(uint32 index, Pair* value)
	{
		BOUNDS_ASSERT(index, mMethodsCount);
		setChild(CHILD_ID_CLASS_NAME + 1 + mParentsCount + index, value); // +1 for name 
	}
	
	INLINE uint32 Class::getMethodsCount() const
	{
		return mMethodsCount;
	}

	INLINE uint32 Class::getMethodNext() const
	{
		return mMethodNext;
	}

	// properties

	INLINE Property* Class::getProperty(uint32 index)
	{
		BOUNDS_ASSERT(index, mPropertiesCount);
		return static_cast<Property*>(getChild(CHILD_ID_CLASS_NAME + 1 + mParentsCount + mMethodsCount + index)); // +1 for name 
	}

	INLINE void Class::setProperty(uint32 index, Property* value)
	{
		BOUNDS_ASSERT(index, mPropertiesCount);
		setChild(CHILD_ID_CLASS_NAME + 1 + mParentsCount + mMethodsCount + index, value); // +1 for name 
	}
	
	INLINE uint32 Class::getPropertiesCount() const
	{
		return mPropertiesCount;
	}

	INLINE uint32 Class::getPropertyNext() const
	{
		return mPropertyNext;
	}

	// name

	INLINE String* Class::getName()
	{
		return static_cast<String*>(getChild(CHILD_ID_CLASS_NAME));
	}

	INLINE void Class::setName(String* value)
	{
		setChild(CHILD_ID_CLASS_NAME, value);
	}
};