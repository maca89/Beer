#pragma once
#include "prereq.h"
#include "NativeThread.h"
#include "Frame.h"
#include "Integer.h"
#include "Float.h"
#include "GenerationalGC.h"
#include "TaskContext.h"


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
	protected:
		VirtualMachine* mVM;
		GenerationalGC* mGC;  // is it needed?
		Heap* mHeap;
		PolymorphicCache* mPolycache; // TODO: in Thread's pool

		TaskContext mTemporaryContext;
		TaskContext* mContext;

		enum
		{
			CREATE_INSTANCE_CACHE_SIZE = 3, // TODO: bigger
		};

	public:
		Thread(VirtualMachine* vm, GenerationalGC * gc);

		virtual ~Thread()
		{
		}

		void setContext(TaskContext* value) { mContext = value; }
		TaskContext* getThreadContext() { return mContext; }

		//volatile bool isSafePoint() const;

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

	protected:
		void staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount = 0);
	};

	/*INLINE volatile bool Thread::isSafePoint() const
	{
		return mVM->isSafePoint();
	}*/
};