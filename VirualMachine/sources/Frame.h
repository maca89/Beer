#pragma once
#include "prereq.h"
#include "Object.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "ArrayStack.h"


namespace Beer
{
	//class Object;
	class Method;

	typedef ArrayStack<Object*> WorkStack;
	//typedef FixedStack<Object*> WorkStack;
	//typedef DynamicStack<Object*> WorkStack;

	class Frame : public Object
	{
	public:
		enum
		{
			FRAME_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT,
		};

	protected:
		uint32 argsCount;
		uint32 frameOffset;
		uint16 vPC;
		WorkStack stack;

		// stack, TODO
		uint32 next;
		uint32 size;

	public:
		INLINE Frame(uint32 argsCount, uint32 stackSize)
			: stack(&mChildren[FRAME_CHILDREN_COUNT], stackSize), vPC(0), frameOffset(0), argsCount(argsCount)
		{
			next = 0;
			size = stackSize;
		}

		INLINE uint16 getProgramCounter()
		{
			return vPC;
		}
		
		INLINE void setProgramCounter(uint16 value)
		{
			vPC = value;
		}

		INLINE uint16 incrProgramCounter()
		{
			return vPC++;
		}

		INLINE uint32 getFrameOffset()
		{
			return frameOffset;
		}

		INLINE uint32 getArgumentsCount()
		{
			return argsCount;
		}

		INLINE void setArgumentsCount(uint32 value)
		{
			argsCount = value;
		}

		INLINE int32 stackPush()
		{
			return translate(stack.push());
		}

		INLINE int32 stackPush(Object* obj)
		{
			return translate(stack.push(obj));
		}

		INLINE void stackPop()
		{
			return stack.pop();
		}

		INLINE Object* stackTop()
		{
			return stack.top();
		}

		INLINE int32 stackTopIndex()
		{
			return translate(stack.topIndex());
		}

		INLINE Object* stackTop(int32 index)
		{
			return stack.top(translate(index));
		}

		INLINE Object** stackTopPtr(int32 index)
		{
			return stack.topPtr(translate(index));
		}
		
		template <typename T>
		INLINE T* stackTop()
		{
			return stack.top<T*>();
		}

		template <typename T>
		INLINE T* stackTop(int32 index)
		{
			return stack.top<T*>(translate(index));
		}

		INLINE void stackMoveTop(int16 count)
		{
			stack.move(count);
		}

		INLINE void stackStore(int16 index, Object* value)
		{
			stack.set(value, translate(index));
		}

		INLINE uint32 stackSize() 
		{
			return stack.size() - frameOffset;
		}

		INLINE void stackCheck(int32 count) // must be signed !!!
		{
			stack.check(count);
		}

		INLINE uint32 translate(int32 index)
		{
			DBG_ASSERT(static_cast<int64>(frameOffset) + argsCount + index >= 0, BEER_WIDEN("Stack index out of bounds"));
			return static_cast<int64>(frameOffset) + argsCount + index;
		}

	//protected:
		INLINE int32 translate(uint32 index)
		{
			return static_cast<int64>(index) - frameOffset - argsCount;
		}

		// mChildren version
		/*static void push(Thread* thread, Frame* receiver, Object* object);
		static Object* top(Thread* thread, Frame* receiver);
		static void pop(Thread* thread, StackRef<Frame> receiver);*/
	};

	/*class FrameProxy
	{
	protected:
		Thread* mThread;
		StackRef<Frame> mFrame;

		INLINE uint32 getProgramCounter()
		{
			return vPC;
		}
		
		INLINE void setProgramCounter(uint32 value)
		{
			vPC = value;
		}

		INLINE uint32 incrProgramCounter()
		{
			return vPC++;
		}

		INLINE uint32 getFrameOffset()
		{
			return frameOffset;
		}

		INLINE uint32 getArgumentsCount()
		{
			return argsCount;
		}

		INLINE void setArgumentsCount(uint32 value)
		{
			argsCount = value;
		}

		INLINE int32 stackPush()
		{
			return translate(stack.push());
		}

		INLINE int32 stackPush(Object* obj)
		{
			return translate(stack.push(obj));
		}

		INLINE void stackPop()
		{
			return stack.pop();
		}

		INLINE Object* stackTop()
		{
			return stack.top();
		}

		INLINE int32 stackTopIndex()
		{
			return translate(stack.topIndex());
		}

		INLINE Object* stackTop(int32 index)
		{
			return stack.top(translate(index));
		}

		INLINE Object** stackTopPtr(int32 index)
		{
			return stack.topPtr(translate(index));
		}
		
		template <typename T>
		INLINE T* stackTop()
		{
			return stack.top<T*>();
		}

		template <typename T>
		INLINE T* stackTop(int32 index)
		{
			return stack.top<T*>(translate(index));
		}

		INLINE void stackMoveTop(int16 count)
		{
			stack.move(count);
		}

		INLINE void stackStore(int16 index, Object* value)
		{
			stack.set(value, translate(index));
		}

		INLINE uint32 stackSize() 
		{
			return stack.size() - frameOffset;
		}

		INLINE void stackCheck(int32 count) // must be signed !!!
		{
			stack.check(count);
		}

		INLINE uint32 translate(int32 index)
		{
			DBG_ASSERT(static_cast<int64>(frameOffset) + argsCount + index >= 0, BEER_WIDEN("Stack index out of bounds"));
			return static_cast<int64>(frameOffset) + argsCount + index;
		}

		INLINE int32 translate(uint32 index)
		{
			return static_cast<int64>(index) - frameOffset - argsCount;
		}
	};*/
};