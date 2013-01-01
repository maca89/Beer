#pragma once
#include "prereq.h"
#include "Object.h"
#include "Selector.h"


namespace Beer
{
	class PropertyReflection;
	class EventReflection;
	class MethodReflection;
	struct CollectedObject;
	class Instance;
	struct GarbageCollector;
	class StackFrame;
	class Object;

	#pragma pack(push, 1)
	class ClassReflection : public Object
	{
	public:
		enum
		{
			FLAG_VALUE_TYPE = 0x01,
			FLAG_INTERFACE = 0x02
		};

		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint8 mFlags;
		uint16 mNameCount;
		char_t* mName;

		uint16 mParentsCount;
		ClassReflection** mParents;

		uint32 mPropertiesCount;
		PropertyReflection** mProperties;

		uint16 mMethodsCount;
		MethodReflection** mMethods;

		uint32 mDefaultChildrenCount;
		////////////////////////////////////////////////////////////

	public:
		INLINE ClassReflection()
		{
		}

		INLINE ~ClassReflection()
		{
			// is never called!
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

		// children count
		INLINE void setDefaultChildrenCount(uint32 value) { mDefaultChildrenCount = value; }
		virtual uint32 getChildrenCount(Object* object) { return mDefaultChildrenCount; }
		
		// name

		INLINE const char_t* getName() const { return (static_cast<const char_t*>(mName)); }

		// parents

		INLINE uint16 getParentsCount() const { return mParentsCount; }
		INLINE void setParent(uint16 i, ClassReflection* parent) { DBG_ASSERT(i < mParentsCount, BEER_WIDEN("Unable to add more parents")); mParents[i] = parent; }
		INLINE ClassReflection* getParent(uint16 i) { DBG_ASSERT(i < mParentsCount, BEER_WIDEN("Unknown parent")); return mParents[i]; }

		// properties

		INLINE uint32 getPropertiesCount() const { return mPropertiesCount; }
		INLINE void setProperty(uint32 i, PropertyReflection* prop) { DBG_ASSERT(i < mPropertiesCount, BEER_WIDEN("Unable to add more properties")); mProperties[i] = prop; }
		INLINE PropertyReflection* getProperty(uint32 i) { DBG_ASSERT(i < mPropertiesCount, BEER_WIDEN("Unknown property")); return mProperties[i]; }

		// mehods

		INLINE uint16 getMethodsCount() const { return mMethodsCount; }
		INLINE void setMethod(uint16 i, MethodReflection* method) { DBG_ASSERT(i < mMethodsCount, BEER_WIDEN("Unable to add more methods")); mMethods[i] = method; }
		INLINE MethodReflection* getMethod(uint16 i) { DBG_ASSERT(i < mMethodsCount, BEER_WIDEN("Unknown method")); return mMethods[i]; }

		// 

		void extends(uint16 i, ClassReflection* klass);
		MethodReflection* findMethod(const char_t* selector);
		bool substituable(ClassReflection* otherClass) const;

		virtual void dump(Object* object, stringstream& out)
		{
			out << getName();
		}

		// instance creation

		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc) = 0 { return NULL; }
		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc);
		/*virtual*/ Object* cloneDeep(Object* object, StackFrame* frame, GarbageCollector* gc);

		template <typename T>
		INLINE T* createInstance(StackFrame* frame, GarbageCollector* gc) { return static_cast<T*>(createInstance(frame, gc));}
		
		//template <typename T>
		//INLINE T* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc) { return static_cast<T*>(cloneShallow(object, frame, gc)); }
		
		template <typename T>
		INLINE T* cloneDeep(Object* object, StackFrame* frame, GarbageCollector* gc) { return static_cast<T*>(cloneShallow(object, frame, gc)); }

		// stuff

		void* operator new(size_t size, void* mem)
		{
			return mem;
		}

		void operator delete(void* mem, void*)
		{
			throw GCException(BEER_WIDEN("delete ClassReflection* should not happen!"));
		}
	};
	#pragma pack(pop)
};