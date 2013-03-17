#pragma once
#include "prereq.h"
#include "Thread.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "Frame.h"
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

	class ClassFileLoader;
	class ClassLoader;
	class Thread;

	class Debugger;

	typedef std::set<Thread*> ThreadSet;
	typedef std::map<string, Class*> ClassReflectionTable;

	class VirtualMachine : protected Thread
	{
	protected:
		//GenerationalGC* mGC;
		ClassReflectionTable mClasses;
		ClassLoader* mClassLoader;
		Debugger* mDebugger;
		ClassTable mClassTable;
		InlineFunctionTable mInlineFnTable;
		ThreadSet mThreads;
		
		// TODO: get rid of these
		Class* mMetaClass;
		Class* mObjectClass;
		Class* mStringClass;
		Class* mFloatClass;
		Class* mIntegerClass;
		Class* mBooleanClass;
		Class* mArrayClass;


	public:
		INLINE VirtualMachine(GenerationalGC* gc)
			: Thread(this, gc),
			mClassLoader(NULL), mDebugger(NULL),
			mMetaClass(NULL), mObjectClass(NULL), mStringClass(NULL), mIntegerClass(NULL), mBooleanClass(NULL), mArrayClass(NULL)
			//mGC(NULL)
		{
		}

		INLINE ~VirtualMachine()
		{
		}

		void loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile);
	
		void addClass(Thread* thread, Class* reflection);
		//void removeClass(ClassReflection* reflection);
		//bool hasClass(string name) const;

		// deprecated, TODO: get rid of
		Class* findClass(StackRef<String> name);

		// deprecated, TODO: get rid of
		Class* findClass(string name);

		//INLINE Heap* getHeap() const { return mHeap; }
		INLINE Debugger* getDebugger() const { return mDebugger; }
		INLINE ClassTable* getClassTable() { return &mClassTable; }
		INLINE InlineFunctionTable* getInlineFunctionTable() { return &mInlineFnTable; }
		INLINE ThreadSet& getThreads() { return mThreads; }
		INLINE ClassLoader* getClassLoader() const { return mClassLoader; }
		INLINE ClassReflectionTable& getClasses() { return mClasses; }

		void init();
		void run();
		void wait();
		void destroy();

		// deprecated, TODO: get rid of these
		String* createString(const string& s);
		Pair* createPair(Object* first, Object* second);
		
		INLINE Class* getMetaClass() { return mMetaClass; }
		INLINE Class* getObjectClass() { return mObjectClass; }
		INLINE Class* _getFloatClass() { return mFloatClass; }
		INLINE Class* _getIntegerClass() { return mIntegerClass; }
		INLINE Class* getBooleanClass() { return mBooleanClass; }
		INLINE Class* getStringClass() { return mStringClass; }
		INLINE Class* getArrayClass() { return mArrayClass; }
		INLINE Class* getPairClass() { return findClass(BEER_WIDEN("Pair")); }

	protected:
		// Thread
		virtual void work();

		
	};
};