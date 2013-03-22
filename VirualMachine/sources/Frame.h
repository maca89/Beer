#pragma once
#include "prereq.h"
#include "Object.h"
//#include "FixedStack.h"
//#include "DynamicStack.h"
//#include "ArrayStack.h"
#include "DownwardStack.h"


namespace Beer
{
	//class Object;
	class Method;

	typedef DownwardStack<Object*> WorkStack;
	//typedef ArrayStack<Object*> WorkStack;
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

		enum
		{
			FRAME_STATE_CHANGED = 0,
			FRAME_STATE_VISITED = 1,

			FRAME_HEAP_ALLOCATED = 2,
			FRAME_STACK_ALLOCATED = 4,
		};

	protected:
		// TODO: flag type
		
		//Frame* previous;
		uint8 mFrameFlags;
		int16 mFrameOffset;
		//uint32 mArgsCount;
		uint16 mVPC;
		void* mIp;
		WorkStack mStack;
		int32 mBonusSpace;

	public:
		INLINE Frame(void* bp, int16 frameOffset, int32 stackSize, int32 bonusSpace)
			: mStack(bp, stackSize), mVPC(0), mIp(NULL), mFrameOffset(frameOffset), mBonusSpace(bonusSpace), mFrameFlags(0)
		{
		}

		INLINE uint8 getFrameFlags() const { return mFrameFlags; }

		INLINE bool hasFrameFlag(uint8 n) const { return (mFrameFlags & n) == n; }
		INLINE void markFrameFlag(uint8 n) { mFrameFlags |= n; }
		
		INLINE void markStackAllocated() { markFrameFlag(FRAME_STACK_ALLOCATED); }
		INLINE bool isStackAllocated() const { return hasFrameFlag(FRAME_STACK_ALLOCATED); }
		
		INLINE void markHeapAllocated() { markFrameFlag(FRAME_HEAP_ALLOCATED); }
		INLINE bool isHeapAllocated() const { return hasFrameFlag(FRAME_HEAP_ALLOCATED); }
		
		INLINE void markChanged() { markFrameFlag(FRAME_STATE_CHANGED); }
		INLINE bool isChanged() const { return hasFrameFlag(FRAME_STATE_CHANGED); }
		
		INLINE void markVisited() { markFrameFlag(FRAME_STATE_VISITED); }
		INLINE bool isVisited() const { return hasFrameFlag(FRAME_STATE_VISITED); }

		INLINE uint16 getProgramCounter()
		{
			return mVPC;
		}
		
		INLINE void setProgramCounter(uint16 value)
		{
			mVPC = value;
		}

		INLINE void* ip()
		{
			return mIp;
		}

		INLINE void ip(void* value)
		{
			mIp = value;
		}

		INLINE uint16 incrProgramCounter()
		{
			return mVPC++;
		}

		INLINE int16 getFrameOffset()
		{
			return mFrameOffset;
		}

		INLINE void setFrameOffset(int16 value)
		{
			mFrameOffset = value;
		}

		INLINE int32 stackPush()
		{
			return translateAbsolute(mStack.push());
		}

		INLINE int32 stackPush(Object* obj)
		{
			return translateAbsolute(mStack.push(obj));
		}

		INLINE void stackPop()
		{
			mStack.pop();
		}

		INLINE Object* stackTop()
		{
			return mStack.top();
		}

		INLINE int32 stackTopIndex()
		{
			return translateAbsolute(mStack.topIndex());
		}

		INLINE Object* stackTop(int32 index)
		{
			return mStack.top(translateRelative(index));
		}

		INLINE Object** stackTopPtr(int32 index)
		{
			return mStack.topPtr(translateRelative(index));
		}
		
		template <typename T>
		INLINE T* stackTop()
		{
			return mStack.top<T*>();
		}

		template <typename T>
		INLINE T* stackTop(int32 index)
		{
			return mStack.top<T*>(translateRelative(index));
		}

		INLINE void stackMoveTop(int32 count)
		{
			mStack.move(count);
		}

		INLINE void stackStore(int32 index, Object* value)
		{
			mStack.set(value, translateRelative(index));
		}

		INLINE uint32 stackLength() 
		{
			return mStack.length();// - frameOffset;
		}

		INLINE void stackCheck(int32 count) // must be signed !!!
		{
			mStack.check(count);
		}

		INLINE void* bp()
		{
			return mStack.bp();
		}

		INLINE void* sp()
		{
			return mStack.sp();
		}

		INLINE Frame* pushFrame(uint32 argsCount, int32 stackSize)
		{
			int32 newFrameSize = sizeof(Frame);
			if(mBonusSpace < newFrameSize)
			{
				return NULL;
			}

			Frame* newFrame = reinterpret_cast<Frame*>(getNewFrameStart());
			memset(newFrame, 0, newFrameSize); // really??

			newFrame->setStaticSize(sizeof(Frame));
			newFrame->setType(NULL); // TODO

			void* newBp = reinterpret_cast<byte*>(mStack.sp());
			new(newFrame) Frame(newBp, 0/*argsCount*/, stackSize, mBonusSpace - newFrameSize - (stackSize * sizeof(Object*)));
			newFrame->markStackAllocated();

			return newFrame;
		}

		INLINE bool popFrame(Frame* frame)
		{
			uint32 newFrameSize = sizeof(Frame);

			if(isTopFrame(frame))
			{
				//free(frameLength);

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

		INLINE bool isContinuous()
		{
			return isStackAllocated();
		}

		INLINE int32 translateAbsolute(int32 index)
		{
			return index - mFrameOffset;
		}

	protected:
		INLINE int32 translateRelative(int32 index)
		{
			//DBG_ASSERT(mFrameOffset + index >= 0, BEER_WIDEN("Stack index out of bounds"));
			return index + mFrameOffset;
		}

		static INLINE uint32 countFrameLength(uint32 stackSize)
		{
			return sizeof(Frame);
		}

		INLINE bool isTopFrame(Frame* frame)
		{
			return getNewFrameStart() == frame;
		}

		INLINE void* getNewFrameStart()
		{
			return getDynamicDataStart(sizeof(Frame));
		}

		// mChildren version
		/*static void push(Thread* thread, Frame* receiver, Object* object);
		static Object* top(Thread* thread, Frame* receiver);
		static void pop(Thread* thread, StackRef<Frame> receiver);*/
	};
};