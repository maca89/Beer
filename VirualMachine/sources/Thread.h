#pragma once
#include "prereq.h"
#include "NativeThread.h"
#include "Frame.h"
#include "Integer.h"
#include "Float.h"
#include "GenerationalGC.h"


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
		//typedef FixedStack<Frame*> FramesStack;

	protected:
		VirtualMachine* mVM;
		GenerationalGC* mGC;  // is it needed?
		Frame* mRootFrame;
		Frame* mTopFrame;
		Heap* mHeap;
		PolymorphicCache* mPolycache; // TODO: in Thread's pool

		enum
		{
			CREATE_INSTANCE_CACHE_SIZE = 3, // TODO: bigger

			DEFAULT_FRAME_SPACE = 4*1024, // 4KB
		};

	public:
		Thread(VirtualMachine* vm, GenerationalGC * gc);

		virtual ~Thread()
		{
		}

		INLINE VirtualMachine* getVM() { return mVM; }
		INLINE GenerationalGC* getGC() { return mGC; } // does every thread need GC?
		INLINE Heap* getHeap() { return mHeap; }
		INLINE Heap* getPermanentHeap() { return mGC->getPermanentHeap(); }

		// TODO: move all Frames to Task
		INLINE Frame* getFrame() { DBG_ASSERT(mTopFrame != NULL, BEER_WIDEN("No stack frame")); return mTopFrame; }
		INLINE bool hasFrame() { return mRootFrame->stackLength() != 0; }
		Frame* openFrame();
		void closeFrame();
		Frame* getPreviousFrame();

		INLINE Frame* getFrames() { return mRootFrame; }

		
		void loadClassFile(ClassFileLoader* loader, ClassFileDescriptor* classFile);

		void getObjectClass(StackRef<Class> ret);
		void getIntegerClass(StackRef<Class> ret);
		void getFloatClass(StackRef<Class> ret);
		void getStringClass(StackRef<Class> ret);
		void getArrayClass(StackRef<Class> ret);
		void getPairClass(StackRef<Class> ret);

		void findClass(StackRef<String> name, StackRef<Class> ret);
		void findMethod(StackRef<Class> klass, StackRef<String> selector, StackRef<Method> ret);

		void createInteger(StackRef<Integer> ret, Integer::IntegerData value);
		void createFloat(StackRef<Float> ret, Float::FloatData value);
		void createString(StackRef<String> ret, string value);
		void createString(StackRef<Integer> length, StackRef<String> ret);
		void createArray(StackRef<Integer> length, StackRef<Array> ret);
		void createPair(StackRef<Object> first, StackRef<Object> second, StackRef<Pair> ret);
		void createInstance(StackRef<Class> klass, StackRef<Object> ret);
		void createPolycache(StackRef<PolymorphicCache> ret, uint16 length);
		
		
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
		void discardFrame(Frame* previousFrame, Frame* currentFrame);
	};
};