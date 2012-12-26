#pragma once
#include "prereq.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "Object.h"
#include "Selector.h"
#include "ObjectClass.h"


namespace Beer
{
	class MethodReflection;
	//typedef FixedStack<Object*> WorkStack;
	typedef DynamicStack<Object*> WorkStack;

	class StackFrame
	{
	public:
		uint32 frameOffset;
		WorkStack* stack;
		MethodReflection* method;
		uint16 programCounter;
		uint16 nextInstruction;
		bool done;

		INLINE StackFrame() : stack(NULL), frameOffset(0), method(NULL), programCounter(0), nextInstruction(0), done(false) {}

		INLINE StackFrame(const StackFrame& f) 
			: stack(f.stack), frameOffset(f.frameOffset), method(f.method), programCounter(f.programCounter), nextInstruction(f.nextInstruction), done(f.done)
		{}
		
		INLINE StackFrame(WorkStack* stack, uint32 stackOffset)
			: stack(stack), frameOffset(stackOffset), method(NULL), programCounter(0), nextInstruction(0), done(false)
		{}

		INLINE ~StackFrame() {}

		INLINE int32 stackPush(Object* obj)
		{
			return translate(stack->push(obj));
		}

		INLINE void stackPop()
		{
			return stack->pop();
		}

		INLINE Object* stackTop()
		{
			return stack->top(0);//translate(stackTopIndex()));
		}

		INLINE int32 stackTopIndex()
		{
			return translate(stack->topIndex());
		}

		INLINE Object* stackTop(int32 index)
		{
			return stack->top(translate(index));
		}

		template <typename T>
		INLINE T* stackTop(int32 index)
		{
			return stack->top<T*>(translate(index));
		}

		/*INLINE int32 stackCopy(int32 index)
		{
			return stackPush(stackTop(index));
		}*/

		INLINE void stackMoveTop(int16 count)
		{
			stack->move(count);
		}

		INLINE void stackStore(int16 index, Object* value)
		{
			stack->set(value, translate(index));
		}

		INLINE uint32 stackSize() 
		{
			return stack->size();
		}

		INLINE uint32 translate(int32 index)
		{
			if(frameOffset == 0) return index;
			return frameOffset + index - 1;
		}

		INLINE int32 translate(uint32 index)
		{
			if(frameOffset == 0) return index;
			return index - frameOffset + 1;
		}
	};
	
	//typedef FixedStack<StackFrame> Frames;
	typedef std::list<StackFrame> Frames;

	#pragma pack(push, 1)
	template <typename T>
	class StackRef
	{
	protected:
		StackFrame* mFrame;
		int32 mIndex;
		
	public:
		INLINE StackRef(StackFrame* frame, int32 index) : mFrame(frame), mIndex(index)
		{
		}
		
		/*template <typename U>
		INLINE StackRef(const StackRef<U>& ref) : mFrame(frame), mIndex(index)
		{
			mFrame->stackStore(mIndex, ref.get());
		}*/

		NOINLINE void operator= (Object* obj)
		{
			mFrame->stackStore(mIndex, obj);
		}

		INLINE void operator= (StackRef& ref)
		{
			mFrame->stackStore(mIndex, ref.get());
		}

		template <typename U>
		INLINE void operator= (StackRef<U>& ref)
		{
			mFrame->stackStore(mIndex, ref.get());
		}

		INLINE T* operator*()
		{
			return get();
		}

		INLINE const T* operator*() const
		{
			return get();
		}

		INLINE const T* operator-> () const
		{
			return get();
		}

		INLINE T* operator-> ()
		{
			return get();
		}

		INLINE uint32 getIndex() const
		{
			return mIndex;
		}

		INLINE void setIndex(int32 index)
		{
			mIndex = index;
		}

		INLINE const T* get() const
		{
			return mFrame->stackTop<T>(mIndex);
		}

		INLINE T* get()
		{
			return mFrame->stackTop<T>(mIndex);
		}

		INLINE void copy()
		{
			mFrame->stackPush(get());
		}

		INLINE bool isNull() const
		{
			return get() == NULL;
		}
	};
	#pragma pack(pop)
};