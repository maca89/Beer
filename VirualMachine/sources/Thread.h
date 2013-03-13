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
		GC* mGC;
		//Frame* mLastFrame;
		//WorkStack mStack;
		//FramesStack mFrames;
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

		// TODO: move all Frames to Task
		INLINE Frame* getFrame() { DBG_ASSERT(mTopFrame != NULL, BEER_WIDEN("No stack frame")); return mTopFrame; }
		INLINE bool hasFrame() { return mRootFrame->stackSize() != 0; }
		Frame* openFrame();
		void closeFrame();
		Frame* getPreviousFrame();

		INLINE Frame* getFrames() { return mRootFrame; }

		void getIntegerClass(StackRef<Class> ret);
		void getFloatClass(StackRef<Class> ret);
		void getStringClass(StackRef<Class> ret);
		void getArrayClass(StackRef<Class> ret);
		void getPairClass(StackRef<Class> ret);

		void getClass(StackRef<String> name, StackRef<Class> ret);
		void getMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> method);

		void createInteger(StackRef<Integer> ret, Integer::IntegerData value);
		void createFloat(StackRef<Float> ret, Float::FloatData value);
		void createString(StackRef<String> ret, string value);
		void createString(StackRef<Integer> length, StackRef<String> ret);
		void createArray(StackRef<Integer> length, StackRef<Array> ret);
		void createPair(StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret);
		void createInstance(StackRef<Class> klass, StackRef<Object> ret);
		
		//INLINE Class* getClass(Reference<String>& name) { return getClass(*name); }
		Class* getClass(const StackRef<Object>& object);

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