#pragma once
#include "prereq.h"
#include "Object.h"
#include "Frame.h"
#include "Pair.h"
#include "Property.h"
#include "String.h" // TODO


namespace Beer
{
	class Method;
	class Frame;
	class VirtualMachine;
	class Thread;

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

			CHILD_ID_CLASS_NAME = OBJECT_CHILDREN_COUNT,
		};

		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint8 mFlags;

		// TODO: garbaged
		uint32 mParentsCount;
		uint32 mPropertiesCount;
		uint32 mMethodsCount;

		uint32 mParentNext;
		uint32 mPropertyNext;
		uint32 mMethodNext;
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

		// parents

		INLINE uint16 getParentsCount() const { return mParentsCount; }
		INLINE uint16 getMethodsCount() const { return mMethodsCount; }
		INLINE uint32 getPropertiesCount() const { return mPropertiesCount; }

		// methods

		static void BEER_CALL createInstance(Thread* thread, StackRef<Class> receiver, StackRef<Object> ret);

		static void BEER_CALL findMethod(Thread* thread, StackRef<Class> receiver, StackRef<String> selector, StackRef<Method> ret);
		static void BEER_CALL substituable(Thread* thread, StackRef<Class> receiver, StackRef<Class> otherClass, StackRef<Boolean> ret);
		
		static void BEER_CALL getName(Thread* thread, StackRef<Class> receiver, StackRef<String> ret);
		static void BEER_CALL setName(Thread* thread, StackRef<Class> receiver, StackRef<String> value);

		static void BEER_CALL getParent(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Class> ret);
		static void BEER_CALL addParent(Thread* thread, StackRef<Class> receiver, StackRef<Class> value);
		
		static void BEER_CALL getProperty(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Property> ret);
		static void BEER_CALL addProperty(Thread* thread, StackRef<Class> receiver, StackRef<Property> value);
		
		static void BEER_CALL getMethod(Thread* thread, StackRef<Class> receiver, StackRef<Integer> index, StackRef<Pair> ret);
		static void BEER_CALL addMethod(Thread* thread, StackRef<Class> receiver, StackRef<Pair> value);
	};
	//#pragma pack(pop)
};