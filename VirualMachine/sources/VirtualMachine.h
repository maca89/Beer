#pragma once
#include "prereq.h"
#include "Thread.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "StackFrame.h"
#include "Integer.h"
#include "Float.h"
#include "String.h"
#include "Character.h"
#include "Boolean.h"
//#include "Pair.h"
#include "ClassTable.h"
#include "InlineFunctionTable.h"


namespace Beer
{
	class Class;
	class Pair;

	class GarbageCollector;
	class ClassFileLoader;
	class ClassLoader;
	class Thread;

	class Debugger;

	//typedef CopyGC Heap;
	typedef std::set<Thread*> ThreadSet;
	typedef std::map<string, Class*> ClassReflectionTable;

	class VirtualMachine : public Thread
	{
	protected:
		//WorkStack* mStack; // get rid of this

		ClassReflectionTable mClasses;
		ClassLoader* mClassLoader;
		Debugger* mDebugger;
		ClassTable mClassTable;
		InlineFunctionTable mInlineFnTable;
		ThreadSet mThreads;
		
		Class* mMetaClass;
		Class* mObjectClass;
		Class* mStringClass;
		Class* mFloatClass;
		Class* mIntegerClass;
		Class* mBooleanClass;
		Class* mPairClass;
		Class* mArrayClass;

	public:
		INLINE VirtualMachine(GC * gc)
			: Thread(this, gc),
			mClassLoader(NULL), mDebugger(NULL),
			mMetaClass(NULL), mObjectClass(NULL), mStringClass(NULL), mIntegerClass(NULL), mBooleanClass(NULL), mPairClass(NULL), mArrayClass(NULL)
		{
		}

		INLINE ~VirtualMachine()
		{
		}
	
		void addClass(Class* reflection);
		//void removeClass(ClassReflection* reflection);
		//bool hasClass(string name) const;
		Class* getClass(String* name);

		INLINE Class* getClass(string name) { return getClass(createString(name)); } // TODO: get rid of
		INLINE Class* getClass(Reference<String>& name) { return getClass(*name); }

		INLINE Class* getClass(const StackRef<Object>& object) const { return mClassTable.translate(object); }
		INLINE Class* getClass(Object* object) const { return mClassTable.translate(object); }

		INLINE Heap* getHeap() const { return mHeap; }
		INLINE Debugger* getDebugger() const { return mDebugger; }
		INLINE ClassTable* getClassTable() { return &mClassTable; }
		INLINE InlineFunctionTable* getInlineFunctionTable() { return &mInlineFnTable; }
		INLINE ThreadSet& getThreads() { return mThreads; }
		INLINE ClassLoader* getClassLoader() const { return mClassLoader; }
		INLINE ClassReflectionTable& getClasses() { return mClasses; }

		void init(uint32 stackInitSize);
		void destroy();

		// TODO: get rid of these
		String* createString(const string& s);
		Pair* createPair(Object* first, Object* second);
		
		INLINE Class* getMetaClass() const { return mMetaClass; }
		INLINE Class* getObjectClass() const { return mObjectClass; }
		INLINE Class* getFloatClass() const { return mFloatClass; }
		INLINE Class* getIntegerClass() const { return mIntegerClass; }
		INLINE Class* getBooleanClass() const { return mBooleanClass; }
		INLINE Class* getStringClass() /*const*/ { return mStringClass; }
		INLINE Class* getArrayClass() const { return mArrayClass; }
		INLINE Class* getPairClass() /*const*/ { return getClass(BEER_WIDEN("Pair")); /*return mPairClass;*/ }

		template <typename T>
		INLINE T* getStringClass() const { return static_cast<T*>(mStringClass); }

		//void createInstance(StackFrame* frame, Class* klass); // pushes onto stack

	protected:
		// Thread
		virtual void work();
	};
};