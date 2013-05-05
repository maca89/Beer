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
#include "TaskScheduler.h"


namespace Beer
{
	class Class;
	class Pair;

	class BytecodeBuilder;
	class ClassFileLoader;
	class ClassLoader;
	class Thread;
	class Debugger;

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
		BytecodeBuilder* mBytecodeBuilder;
		TaskScheduler mScheduler;

		// TODO: get rid of these
		Class* mMetaClass;
		Class* mMethodClass;
		Class* mPropertyClass;
		Class* mFrameClass;
		Class* mObjectClass;
		Class* mStringClass;
		Class* mFloatClass;
		Class* mIntegerClass;
		Class* mBooleanClass;
		Class* mArrayClass;
		Class* mPoolClass;

		// TODO: get rid of this
		TaskContext mTemporaryContext;


	public:
		INLINE VirtualMachine(GenerationalGC* gc)
			: Thread(666, this, gc),
			mClassLoader(NULL), mDebugger(NULL),
			mMetaClass(NULL), mObjectClass(NULL), mStringClass(NULL), mIntegerClass(NULL), mBooleanClass(NULL), mArrayClass(NULL), mPoolClass(NULL), 
			mMethodClass(NULL), mPropertyClass(NULL), mFrameClass(NULL), mBytecodeBuilder(NULL)//, mGC(NULL)
		{
		}

		INLINE ~VirtualMachine()
		{
		}

		void loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile);
	
		void addClass(Thread* thread, Class* reflection);
		Class* findClass(StackRef<String> name);
		Class* findClass(string name);

		//INLINE Heap* getHeap() const { return mHeap; }
		//INLINE GenerationalGC* getGC() { return mGC; }
		INLINE Debugger* getDebugger() const { return mDebugger; }
		INLINE ClassTable* getClassTable() { return &mClassTable; }
		INLINE InlineFunctionTable* getInlineFunctionTable() { return &mInlineFnTable; }
		INLINE ClassLoader* getClassLoader() const { return mClassLoader; }
		INLINE ClassReflectionTable& getClasses() { return mClasses; }
		INLINE BytecodeBuilder* getBytecodeBuilder() { return mBytecodeBuilder; }
		INLINE TaskScheduler* getScheduler() { return &mScheduler; }

		void init(uint32 numberOfThreads);
		void run();
		//void wait();
		void destroy();
		
		INLINE Class* getMetaClass() { return mMetaClass; }
		INLINE Class* getMethodClass() { return mMethodClass; }
		INLINE Class* getPropertyClass() { return mPropertyClass; }
		INLINE Class* getFrameClass() { return mFrameClass; }
		INLINE Class* getObjectClass() { return mObjectClass; }
		INLINE Class* getFloatClass() { return mFloatClass; }
		INLINE Class* getIntegerClass() { return mIntegerClass; }
		INLINE Class* getBooleanClass() { return mBooleanClass; }
		INLINE Class* getStringClass() { return mStringClass; }
		INLINE Class* getArrayClass() { return mArrayClass; }
		INLINE Class* getPoolClass() { return mPoolClass; }
		INLINE Class* getPairClass() { return findClass(BEER_WIDEN("Pair")); }
		
		INLINE void setMetaClass(Class* value) { mMetaClass = value; }
		INLINE void setMethodClass(Class* value) { mMethodClass = value; }
		INLINE void setPropertyClass(Class* value) { mPropertyClass = value; }
		INLINE void setFrameClass(Class* value) { mFrameClass = value; }
		INLINE void setObjectClass(Class* value) { mObjectClass = value; }
		INLINE void setFloatClass(Class* value) { mFloatClass = value; }
		INLINE void setIntegerClass(Class* value) { mIntegerClass = value; }
		INLINE void setBooleanClass(Class* value) { mBooleanClass = value; }
		INLINE void setStringClass(Class* value) { mStringClass = value; }
		INLINE void setArrayClass(Class* value) { mArrayClass = value; }
		INLINE void setPoolClass(Class* value) { mPoolClass = value; }

		//volatile bool isSafePoint() const;
		void startSafePoint();
		void stopSafePoint();

	protected:
		// Thread
		virtual void work();

		void fixFramesClasses();
	};


	/*INLINE volatile bool VirtualMachine::isSafePoint() const
	{
		return mScheduler.isSafePoint();
	}*/

	INLINE void VirtualMachine::startSafePoint()
	{
		//cout << BEER_WIDEN("[SafePoint started]\n");
		mScheduler.startSafePoint();
	}
	
	INLINE void VirtualMachine::stopSafePoint()
	{
		//cout << BEER_WIDEN("[SafePoint stopped]\n");
		mScheduler.stopSafePoint();
	}
};