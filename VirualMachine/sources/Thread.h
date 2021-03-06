#pragma once
#include "prereq.h"
#include "NativeThread.h"
#include "Frame.h"
#include "Integer.h"
#include "Float.h"
#include "String.h"
#include "GenerationalGC.h"
#include "Task.h"
#include "TaskContext.h"
#include "SynchronizationEvent.h"


namespace Beer
{
	class VirtualMachine;
	class Array;
	class GenerationalGC;
	class Heap;
	class Pair;
	class ClassFileLoader;
	class ClassFileDescriptor;
	class PolymorphicCache;
	class Pool;

	class Thread : public NativeThread
	{
	public:
		enum
		{
			CREATE_INSTANCE_CACHE_SIZE = 3, // TODO: bigger
		};

	protected:
		VirtualMachine* mVM;
		GenerationalGC* mGC;  // is it needed?
		Heap* mHeap;

		Task* mTask;
		TaskContext* mContext;
		uint16 mThreadId;
		volatile bool mExecutionPaused;

	public:
		Thread(uint16 threadId, VirtualMachine* vm, GenerationalGC * gc);

		virtual ~Thread()
		{
		}

		uint16 getThreadId() const;

		void setContext(TaskContext* cxt);
		void setContext(Task* task);
		TaskContext* getThreadContext() const;
		Task* getTask() const;

		volatile bool isExecutionPaused() const;
		void pauseExecution();

		INLINE VirtualMachine* getVM() { return mVM; }
		INLINE GenerationalGC* getGC() { return mGC; } // does every thread need GC?
		INLINE Heap* getHeap() { return mHeap; }
		INLINE Heap* getPermanentHeap() { return mGC->getPermanentHeap(); }

		// TODO: get rid of
		INLINE Frame* getFrame() { return mContext->getFrame(); }
		INLINE bool hasFrame() { return mContext->hasFrame(); }
		INLINE Frame* openFrame() { return mContext->openFrame(); }
		INLINE void closeFrame() { mContext->closeFrame(); }
		
		void loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile);

		void getObjectClass(StackRef<Class> ret);
		void getIntegerClass(StackRef<Class> ret);
		void getFloatClass(StackRef<Class> ret);
		void getStringClass(StackRef<Class> ret);
		void getArrayClass(StackRef<Class> ret);
		void getPairClass(StackRef<Class> ret);
		void getPoolClass(StackRef<Class> ret);

		void findClass(StackRef<String> name, StackRef<Class> ret);
		void findMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret);

		Integer* createConstantInteger(Integer::IntegerData value);
		Float* createConstantFloat(Float::FloatData value);
		String* createConstantString(String::LengthData length);
		String* createConstantString(const String::CharacterData* cstring);

		void createInteger(StackRef<Integer> ret, Integer::IntegerData value);
		void createFloat(StackRef<Float> ret, Float::FloatData value);
		void createString(StackRef<String> ret, string value);
		void createString(StackRef<Integer> length, StackRef<String> ret);
		void createString(StackRef<String> ret, Integer::IntegerData length);
		void createArray(StackRef<Integer> length, StackRef<Array> ret);
		void createPair(StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret);
		void createInstance(StackRef<Class> klass, StackRef<Object> ret);
		void createPolycache(StackRef<PolymorphicCache> ret, uint16 length);
		void createPool(StackRef<Pool> ret, uint16 length);
		
		Class* getType(Object* object);
		Class* getType(StackRef<Object> object);
		void getType(StackRef<Object> object, StackRef<Class> ret);

		void init();
		bool trampoline();

	protected:
		Object* staticCreateConstant(Class* klass, int32 staticSize, int32 additionalChildrenCount = 0);
		void staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount = 0);
	};

	INLINE volatile bool Thread::isExecutionPaused() const
	{
		return mExecutionPaused;
	}

	INLINE void Thread::pauseExecution()
	{
		mExecutionPaused = true;
	}

	// inline definitions

	INLINE uint16 Thread::getThreadId() const
	{
		return mThreadId;
	}

	INLINE Class* Thread::getType(StackRef<Object> object)
	{
		return getType(*object);
	}

	INLINE void Thread::setContext(TaskContext* cxt)
	{
		mTask = NULL;
		mContext = cxt;
	}

	INLINE void Thread::setContext(Task* task)
	{
		mTask = task;
		mContext = mTask->getContext();
		mContext->setHeap(getHeap());
	}
	
	INLINE Task* Thread::getTask() const
	{
		return mTask;
	}
	
	INLINE TaskContext* Thread::getThreadContext() const
	{
		return mContext; 
	}
};