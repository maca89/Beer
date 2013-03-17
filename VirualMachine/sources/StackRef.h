#pragma once
#include "prereq.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "Frame.h"


namespace Beer
{	
#ifdef BEER_STACK_DEBUGGING
	#pragma pack(push, 1)
	template <typename T>
	class StackRef
	{
	protected:
		Frame* mFrame;
		int32 mIndex;
		
	public:
		INLINE StackRef(Frame* frame, int32 index) : mFrame(frame), mIndex(index)
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
			NULL_ASSERT(get());
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

		INLINE operator StackRef<Object>()
		{
			return staticCast<Object>();
		}

		template<typename U>
		INLINE StackRef<U> staticCast()
		{
			static_cast<U*>(reinterpret_cast<T*>(NULL)); // check if can be cast
			return StackRef<U>(mFrame, mIndex);
		}

		INLINE StackRef push(Frame* frame)
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
		INLINE StackRef(Frame* frame, int32 index)
		{
			static_cast<T*>(reinterpret_cast<Object*>(NULL)); // check if can be cast
			mPtr = frame->stackTopPtr(index);
		}

		INLINE StackRef(T** object)
		{
			static_cast<T*>(reinterpret_cast<Object*>(NULL)); // check if can be cast
			mPtr = reinterpret_cast<Object**>(object);
		}

		INLINE void operator= (T* obj) { set(obj); }
		INLINE void operator= (StackRef& ref) { set(ref.get()); }

		INLINE T* operator*() { return get(); }
		INLINE const T* operator*() const { return get();}

		INLINE T* operator-> ()
		{
			NULL_ASSERT(get());
			return get();
		}

		INLINE const T* get() const { return reinterpret_cast<T*>(*mPtr); }
		INLINE T* get() { return reinterpret_cast<T*>(*mPtr); }
		INLINE void set(T* object) { *mPtr = object; }
		INLINE bool isNull() const { return get() == NULL; }

		INLINE Object** getPtr() { return mPtr; }

		INLINE operator StackRef<Object>() { return staticCast<Object>(); }

		template<typename U>
		INLINE StackRef<U> staticCast()
		{
			static_cast<U*>(reinterpret_cast<T*>(NULL)); // check if can be cast
			return StackRef<U>(reinterpret_cast<U**>(mPtr));
		}

		INLINE StackRef push(Frame* frame) { return StackRef(frame, frame->stackPush(get())); }
	};
	#pragma pack(pop)
#endif // BEER_STACK_DEBUGGING
};