#pragma once
#include "prereq.h"
#include "Object.h"
#include "Frame.h"
#include "Method.h"
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
			FLAG_ABSTRACT = 0x04,
			FLAG_SEALED = 0x08,

			FLAG_ENTRYPOINT = 0x10,

			CLASS_METHODS_COUNT = OBJECT_METHODS_COUNT + 2, // substituable, getName
			CLASS_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + CLASS_METHODS_COUNT,

			METHOD_SLOT_CREATE_INSTANCE = 0,
			METHOD_SLOT_OBJECT_CTOR = 1,
			METHOD_SLOT_OPERATOR_STRING = 2,
		};

		typedef void (*Traverser)(TraverseObjectReceiver* receiver, Class* klass, Object* instance);
		//typedef void (*TraverseClassFn)(TraverseObjectReceiver* receiver, Class* klass);

		enum
		{
			// children order (not including those of Object parent class):
			// #1 interfaces[]
			// #2 methods[]
			// #3 properties[]
		};

		friend class MetaClass;

	//protected:
		uint8 mFlags;
		Class* mSuperClass;
		String* mName;
		uint32 mInstanceStaticSize;
		Traverser mTraverser;

		uint32 mInterfacesCount;
		uint32 mPropertiesCount;
		uint32 mVirtualMethodsCount;
		uint32 mInterfaceMethodsCount;
		
		uint32 mInterfaceNext;
		uint32 mPropertyNext;
		uint32 mVirtualMethodNext;
		uint32 mInterfaceMethodNext;

		Object** mVTable;

	public:
		Class(uint32 interfaces, uint32 properties, uint32 virtualMethods, uint32 interfaceMethods);

		// flags
		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }
		
		INLINE void markAsValueType() { markFlag(FLAG_VALUE_TYPE); }
		INLINE bool isValueType() const { return hasFlag(FLAG_VALUE_TYPE); }
		
		INLINE void markInterface() { markFlag(FLAG_INTERFACE); }
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
		//bool hasParentFreeSlot() const;

		String* getName();
		void setName(String* value);

		static String::LengthData getFQNFromSelector(String* selector);
		
		Property* getProperty(uint32 index);
		uint32 getPropertiesCount() const;
		uint32 getPropertyNext() const;
		void addProperty(Property* value);
		void setProperty(uint32 index, Property* value);

		// parents
		
		//Class* getParent(uint32 index); // TODO: protected
		//uint32 getParentsCount() const; // deprecated

		Class* getSuperClass();
		void setSuperClass(Class* klass);

		// interfaces

		uint32 addInterface(Class* interf); // returns index to the list of interfaces
		Class* getInterface(uint32 index);
		bool hasInterface(Class* interf);
		uint32 getInterfacesCount() const;
		bool findInterfaceIndex(Class* interf, uint32& out_index); // find interface index (not parent index!!!)
		bool findInterfaceMethodsStart(Class* interf, uint32& out_index); // find methods indices start

		// virtual table

		uint32 getVirtualMethodsCount() const;
		uint32 getInterfaceMethodsCount() const;
		
		Method* getMethod(uint32 index);
		void setMethod(uint32 index, Method* value);
		uint32 getMethodsCount() const;

		uint32 addVirtualMethod(Method* method);

		bool findVirtualMethodIndex(String* selector, uint32& out_index);
		bool findInterfaceMethodIndex(Class* interf, String* selector, uint32& out_index);

		Method* findMethod(String* selector); // deprecated
		Method* findVirtualMethod(String* selector);
		Method* findInterfaceMethod(Class* interf, String* selector);

		// instance

		uint32 getInstanceStaticSize() const;

		// 

		bool substituable(Class* otherClass);

		// static methods

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret);
		static void BEER_CALL substituable(Thread* thread, StackRef<Class> receiver, StackRef<Class> otherClass, StackRef<Boolean> ret);
		static void BEER_CALL getName(Thread* thread, StackRef<Class> receiver, StackRef<String> ret);

		// traversers
		static void DefaultInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance);

	protected:
		uint32 addInterfaceMethod(Method* method);
		//void setParent(uint32 index, Class* value);
		//uint32 getInterfaceMethodsStart() const;
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

	/*INLINE bool Class::hasInterfaceFreeSlot() const
	{
		return mParentNext < mParentsCount;
	}*/

	// parents

	INLINE Class* Class::getSuperClass()
	{
		return mSuperClass;
	}

	INLINE Class* Class::getInterface(uint32 index)
	{
		BOUNDS_ASSERT(index, mInterfacesCount);
		return static_cast<Class*>(getChild(OBJECT_CHILDREN_COUNT + index));
	}

	INLINE uint32 Class::getInterfacesCount() const
	{
		return mInterfacesCount;
	}

	// methods

	INLINE uint32 Class::addVirtualMethod(Method* method)
	{
		BOUNDS_ASSERT(mVirtualMethodNext, mVirtualMethodsCount);
		uint32 index = mVirtualMethodNext++;
		setMethod(index, method);
		return index;
	}

	INLINE uint32 Class::addInterfaceMethod(Method* method)
	{
		BOUNDS_ASSERT(mInterfaceMethodNext, mInterfaceMethodsCount);
		uint32 index = mVirtualMethodsCount + (mInterfaceMethodNext++);
		setMethod(index, method);
		return index;
	}

	INLINE Method* Class::getMethod(uint32 index)
	{
		DBG_ASSERT(index < getMethodsCount(), BEER_WIDEN("Tried to get method with index out of bound")); // dbg assert due to speed
		//return static_cast<Method*>(getChild(OBJECT_CHILDREN_COUNT + getInterfacesCount() + index));
		return static_cast<Method*>(mVTable[index]);
	}

	INLINE void Class::setMethod(uint32 index, Method* value)
	{
		BOUNDS_ASSERT(index, getMethodsCount());
		//setChild(OBJECT_CHILDREN_COUNT + getInterfacesCount() + index, value);
		mVTable[index] = value;
		value->setSlotId(index);
	}
	
	INLINE uint32 Class::getMethodsCount() const
	{
		return getVirtualMethodsCount() + getInterfaceMethodsCount();
	}
	
	INLINE uint32 Class::getVirtualMethodsCount() const
	{
		return mVirtualMethodsCount;
	}

	INLINE bool Class::hasInterface(Class* interf)
	{
		uint32 index = 0;
		if(findInterfaceIndex(interf, index))
		{
			return true;
		}
		return false;
	}

	INLINE uint32 Class::getInterfaceMethodsCount() const
	{
		return mInterfaceMethodsCount;
	}

	// properties

	INLINE Property* Class::getProperty(uint32 index)
	{
		BOUNDS_ASSERT(index, mPropertiesCount);
		return static_cast<Property*>(getChild(OBJECT_CHILDREN_COUNT + getInterfacesCount() + getMethodsCount() + index));
	}

	INLINE void Class::setProperty(uint32 index, Property* value)
	{
		BOUNDS_ASSERT(index, mPropertiesCount);
		setChild(OBJECT_CHILDREN_COUNT + getInterfacesCount() + getMethodsCount() + index, value);
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
		return mName;
	}

	INLINE void Class::setName(String* value)
	{
		mName = value;
	}

	INLINE Method* Class::findMethod(String* selector)
	{
		return findVirtualMethod(selector);
	}

	INLINE Method* Class::findVirtualMethod(String* selector)
	{
		uint32 index = 0;
		if(findVirtualMethodIndex(selector, index))
		{
			return getMethod(index);
		}
		return NULL;
	}

	INLINE Method* Class::findInterfaceMethod(Class* interf, String* selector)
	{
		uint32 index = 0;
		if(findInterfaceMethodIndex(interf, selector, index))
		{
			return getMethod(index);
		}
		return NULL;
	}

	// instance

	INLINE uint32 Class::getInstanceStaticSize() const
	{
		return mInstanceStaticSize;
	}
};