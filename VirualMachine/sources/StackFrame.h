#pragma once
#include "prereq.h"
#include "FixedStack.h"
#include "DynamicStack.h"
//#include "Object.h"


namespace Beer
{
	class Object;
	class Method;
	//typedef FixedStack<Object*> WorkStack;
	typedef DynamicStack<Object*> WorkStack;

	class StackFrame
	{
	public:
		uint32 frameOffset;
		StackFrame* prev;
		WorkStack* stack;
		Method* method;
		//uint16 programCounter;
		byte* ip;
		uint32 vPC;

		INLINE StackFrame() : prev(NULL), stack(NULL), frameOffset(0), method(NULL), /*programCounter(0),*/ ip(NULL), vPC(0)
		{
		}

		INLINE StackFrame(WorkStack* stack)
			: prev(NULL), stack(stack), frameOffset(stack->size()), method(NULL), /*programCounter(0),*/ ip(NULL), vPC(0)
		{
		}
		
		INLINE StackFrame(StackFrame* prev)
			: prev(prev), stack(prev->stack), frameOffset(prev->stack->size()), method(NULL), /*programCounter(0),*/ ip(NULL), vPC(0)
		{
		}

		INLINE ~StackFrame() {}

		INLINE int32 stackPush()
		{
			return translate(stack->push());
		}

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
			return stack->top(0);
		}

		INLINE int32 stackTopIndex()
		{
			return translate(stack->topIndex());
		}

		INLINE Object* stackTop(int32 index)
		{
			return stack->top(translate(index));
		}

		INLINE Object** stackTopPtr(int32 index)
		{
			return stack->topPtr(translate(index));
		}

		template <typename T>
		INLINE T* stackTop(int32 index)
		{
			return stack->top<T*>(translate(index));
		}

		INLINE void stackMoveTop(int16 count)
		{
			/*if(count > 1000)
			{
				int a = 0;
			}*/
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

		INLINE uint32 translate(int32 index) // index <= 0
		{
			/*if(static_cast<int64>(frameOffset) + index <= 0)
			{
				int a = 0;
			}*/
			DBG_ASSERT(static_cast<int64>(frameOffset) + index > 0, BEER_WIDEN("Stack index out of bounds"));
			return static_cast<int64>(frameOffset) + index - 1;
		}

		INLINE int32 translate(uint32 index)
		{
			return static_cast<int64>(index) - frameOffset + 1;
		}
	};
	
#ifdef BEER_STACK_DEBUGGING
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

		INLINE void operator= (T* obj)
		{
			set(obj);
		}

		INLINE void operator= (StackRef& ref)
		{
			set(ref.get());
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

		INLINE T* operator-> ()
		{
			DBG_ASSERT(get() != NULL
#ifdef BEER_VALUE_TYPES_WORKAROUND
				|| Object::isInlineValue(get()), BEER_WIDEN("Null pointer")); // Object::isInlineValue is just a workaround TODO: not checking inline value
#endif // BEER_VALUE_TYPES_WORKAROUND
			return get();
		}

		INLINE int32 getIndex() const
		{
			return mIndex;
		}

		INLINE const T* get() const
		{
			return mFrame->stackTop<T>(mIndex);
		}

		INLINE T* get()
		{
			return mFrame->stackTop<T>(mIndex);
		}

		INLINE void set(T* object)
		{
			mFrame->stackStore(mIndex, object);
		}

		INLINE bool isNull() const
		{
			return get() == NULL;
		}

		template<typename U>
		INLINE StackRef<U> staticCast()
		{
			static_cast<U*>(reinterpret_cast<T*>(NULL)); // check if can be cast
			return StackRef<U>(mFrame, mIndex);
		}

		INLINE StackRef push(StackFrame* frame)
		{
			return StackRef(frame, frame->stackPush(get()));
		}
	};
	#pragma pack(pop)
#else
	#pragma pack(push, 1)
	template <typename T>
	class StackRef
	{
	protected:
		Object** mPtr;
		
	public:
		INLINE StackRef(StackFrame* frame, int32 index)
		{
			static_cast<T*>(reinterpret_cast<Object*>(NULL)); // check if can be cast
			mPtr = frame->stackTopPtr(index);
		}

		INLINE StackRef(T** object) { mPtr = object; }

		INLINE void operator= (T* obj) { set(obj); }
		INLINE void operator= (StackRef& ref) { set(ref.get()); }

		INLINE T* operator*() { return get(); }
		INLINE const T* operator*() const { return get();}

		INLINE T* operator-> ()
		{
			// Object::isInlineValue is just a workaround TODO: not checking inline value
			DBG_ASSERT(get() != NULL
#ifdef BEER_VALUE_TYPES_WORKAROUND
				|| Object::isInlineValue(get()), BEER_WIDEN("Null pointer")
#endif // BEER_VALUE_TYPES_WORKAROUND
			);
			return get();
		}

		INLINE const T* get() const { return reinterpret_cast<T*>(*mPtr); }
		INLINE T* get() { return reinterpret_cast<T*>(*mPtr); }
		INLINE void set(T* object) { *mPtr = object; }
		INLINE bool isNull() const { return get() == NULL; }

		INLINE Object** getPtr() { return mPtr; }

		template<typename U>
		INLINE StackRef<U> staticCast()
		{
			static_cast<U*>(reinterpret_cast<T*>(NULL)); // check if can be cast
			return StackRef<U>(reinterpret_cast<U**>(mPtr));
		}

		INLINE StackRef push(StackFrame* frame) { return StackRef(frame, frame->stackPush(get())); }
	};
	#pragma pack(pop)
#endif // BEER_STACK_DEBUGGING
};