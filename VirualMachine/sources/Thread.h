#pragma once
#include "prereq.h"
#include "NativeThread.h"
#include "Frame.h"
#include "Integer.h"
#include "Float.h"
#include "PolymorphicInlineCache.h"


namespace Beer
{
	class VirtualMachine;
	class Array;
	class GenerationalGC;
	class Heap;
	class Pair;

	typedef GenerationalGC GC;

	class Thread : public NativeThread
	{
	public:
		//typedef FixedStack<Frame*> FramesStack;

	protected:
		VirtualMachine* mVM;
		GC* mGC;  // is it needed?
		Frame* mRootFrame;
		Frame* mTopFrame;
		Heap* mHeap;

		enum
		{
			CREATE_INSTANCE_CACHE_SIZE = 10
		};
		byte createInstanceMethodCacheBytes[sizeof(PolymorphicInlineCache::CachedMethod) * CREATE_INSTANCE_CACHE_SIZE];

	public:
		Thread(VirtualMachine* vm, GC * gc);

		virtual ~Thread()
		{
		}

		//INLINE WorkStack* getStack() { return &mStack; }

		INLINE VirtualMachine* getVM() { return mVM; }

		INLINE GC* getGC() { return mGC; } // does every thread need GC?
		
		INLINE Heap* getHeap() { return mHeap; }
		INLINE Heap* getPermanentHeap() { return mGC->getPermanentHeap(); }

		// TODO: move all Frames to Task
		INLINE Frame* getFrame() { DBG_ASSERT(mTopFrame != NULL, BEER_WIDEN("No stack frame")); return mTopFrame; }
		INLINE bool hasFrame() { return mRootFrame->stackSize() != 0; }
		Frame* openFrame();
		void closeFrame();
		Frame* getPreviousFrame();

		INLINE Frame* getFrames() { return mRootFrame; }

		void getObjectClass(StackRef<Class> ret);
		void _getIntegerClass(StackRef<Class> ret); // deprecated
		void _getFloatClass(StackRef<Class> ret); // deprecated
		void getStringClass(StackRef<Class> ret);
		void getArrayClass(StackRef<Class> ret);
		void getPairClass(StackRef<Class> ret);

		void findClass(StackRef<String> name, StackRef<Class> ret);
		void findClass(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret);

		void createInteger(StackRef<Integer> ret, Integer::IntegerData value);
		void createFloat(StackRef<Float> ret, Float::FloatData value);
		void createString(StackRef<String> ret, string value);
		void createString(StackRef<Integer> length, StackRef<String> ret);
		void createArray(StackRef<Integer> length, StackRef<Array> ret);
		void createPair(StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret);
		void createInstance(StackRef<Class> klass, StackRef<Object> ret);
		
		
		Class* getType(StackRef<Object> object); // deprecated
		void getType(StackRef<Object> object, StackRef<Class> ret);

		void init();

	protected:
		void staticCreateObject(StackRef<Class> klass, StackRef<Object> ret, int32 staticSize, int32 additionalChildrenCount = 0);

		INLINE void fetchTopFrame()
		{
			if(hasFrame()) mTopFrame = mRootFrame->stackTop<Frame>();
			else mTopFrame = NULL;
		}

		Frame* allocFrame(uint32 stackSize, uint32 argsCout);
	};
};