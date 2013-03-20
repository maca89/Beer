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
			//FRAME_HEADER = OBJECT_HEADER + 
		};

	protected:
		// TODO: flag type
		
		//uint32 frameOffset;
		uint32 mArgsCount;
		uint16 mVPC;
		WorkStack mStack;

		uint32 mAfterNext;
		uint32 mAfterSpace;

	public:
		INLINE Frame(uint32 argsCount, uint32 stackSize, uint32 afterSpace)
			: mStack(getChildren(sizeof(Frame)), stackSize), mVPC(0)/*, frameOffset(0)*/, mArgsCount(argsCount), mAfterSpace(afterSpace), mAfterNext(0)
		{
			/*if(mStaticSize > 5*1024)
			{
				int a = 0;
			}*/
		}

		INLINE uint16 getProgramCounter()
		{
			return mVPC;
		}
		
		INLINE void setProgramCounter(uint16 value)
		{
			mVPC = value;
		}

		INLINE uint16 incrProgramCounter()
		{
			return mVPC++;
		}

		/*INLINE uint32 getFrameOffset()
		{
			return frameOffset;
		}*/

		INLINE uint32 getArgumentsCount()
		{
			return mArgsCount;
		}

		INLINE void setArgumentsCount(uint32 value)
		{
			mArgsCount = value;
		}

		INLINE int32 stackPush()
		{
			return translate(mStack.push());
		}

		INLINE int32 stackPush(Object* obj)
		{
			return translate(mStack.push(obj));
		}

		INLINE void stackPop()
		{
			return mStack.pop();
		}

		INLINE Object* stackTop()
		{
			return mStack.top();
		}

		INLINE int32 stackTopIndex()
		{
			return translate(mStack.topIndex());
		}

		INLINE Object* stackTop(int32 index)
		{
			return mStack.top(translate(index));
		}

		INLINE Object** stackTopPtr(int32 index)
		{
			return mStack.topPtr(translate(index));
		}
		
		template <typename T>
		INLINE T* stackTop()
		{
			return mStack.top<T*>();
		}

		template <typename T>
		INLINE T* stackTop(int32 index)
		{
			return mStack.top<T*>(translate(index));
		}

		INLINE void stackMoveTop(int16 count)
		{
			mStack.move(count);
		}

		INLINE void stackStore(int16 index, Object* value)
		{
			mStack.set(value, translate(index));
		}

		INLINE uint32 stackSize() 
		{
			return mStack.size();// - frameOffset;
		}

		INLINE uint32 stackRealSize() 
		{
			return mStack.realSize();
		}

		INLINE void stackCheck(int32 count) // must be signed !!!
		{
			mStack.check(count);
		}

		INLINE uint32 translate(int32 index)
		{
			DBG_ASSERT(/*static_cast<int64>(frameOffset) +*/ mArgsCount + index >= 0, BEER_WIDEN("Stack index out of bounds"));
			return /*static_cast<int64>(frameOffset) +*/ mArgsCount + index;
		}

	//protected:
		INLINE int32 translate(uint32 index)
		{
			return static_cast<int64>(index) /*- frameOffset*/ - mArgsCount;
		}

		INLINE Frame* pushFrame(uint32 argsCount, uint32 stackSize)
		{
			uint32 newFrameLength = countFrameLength(stackSize);
			if(!canAlloc(newFrameLength))
			{
				return NULL;
			}

			Frame* newFrame = reinterpret_cast<Frame*>(alloc(newFrameLength));
			memset(newFrame, 0, newFrameLength);

			newFrame->setStaticSize(sizeof(Frame));
			newFrame->setType(NULL); // TODO

			new(newFrame) Frame(argsCount, stackSize, countFreeSpace());
			/*if(newFrame->mAfterSpace > 2048)
			{
				int a = 0;
			}*/
			return newFrame;
		}

		INLINE bool popFrame(Frame* frame)
		{
			uint32 frameLength = countFrameLength(frame->stackRealSize());

			if(isTopFrame(frame, frameLength))
			{
				free(frameLength);
				//cout << "Frame " << frame << " popped, length:" << frameLength << "\n";
				return true;
			}
			else
			{
				//cout << "Frame " << frame << " not popped, length:" << frameLength << "\n";
			}

			return false;
		}

		/*INLINE Frame* getTopFrame()
		{
			if(mAfterNext == 0) return NULL;
		}*/

		uint32 getAfterSpace() { return mAfterSpace; } // dbg

	protected:
		INLINE uint32 countFrameLength(uint32 stackSize)
		{
			return sizeof(Frame) + (stackSize * sizeof(Object*));
		}

		INLINE uint32 countFreeSpace() const
		{
			return mAfterSpace - mAfterNext;
		}

		INLINE bool canAlloc(uint32 length)
		{
			return countFreeSpace() >= length;
		}

		INLINE void* alloc(uint32 length)
		{
			DBG_ASSERT(canAlloc(length), BEER_WIDEN("Cannot allocate on frame stak"));
			void* ptr = getAfterDataStart();
			mAfterNext += length;
			return ptr;
		}

		INLINE void free(uint32 length)
		{
			DBG_ASSERT(mAfterNext >= length, BEER_WIDEN("Cannot free from frame stack"));
			mAfterNext -= length;
		}

		INLINE bool isTopFrame(Frame* frame, uint32 frameLength)
		{
			if(mAfterNext < frameLength)
			{
				//cout << "Not a top frame: " << frame << ", length:" << frameLength << "\n";
				return false;
			}

			void* start = getAfterDataStart();
			Frame* top = reinterpret_cast<Frame*>(reinterpret_cast<byte*>(start) - frameLength);

			if(top != frame)
			{
				//cout << "Not a top frame: " << frame << ", length:" << frameLength << "\n";
			}

			return top == frame;
		}

		INLINE void* getAfterDataStart()
		{
			return reinterpret_cast<byte*>(getDynamicDataStart(sizeof(Frame))) + (mStack.realSize() * sizeof(Object*)) + mAfterNext;
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