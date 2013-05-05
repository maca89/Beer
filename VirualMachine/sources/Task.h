#pragma once
#include "prereq.h"
#include "Object.h"
#include "Boolean.h"
#include "Integer.h"
#include "TaskContext.h"
#include "InterlockedQueue.h"


namespace Beer
{
	class VirtualMachine;
	class Frame;

	class Task : public Object
	{
	public:
		enum
		{
			TASK_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT,
			TASK_METHODS_COUNT = OBJECT_METHODS_COUNT + 8,

			TASK_STATE_SCHEDULED = 1<<1,
			TASK_STATE_COMPLETED = 1<<2,
			TASK_STATE_CANCELED = 1<<3,
			TASK_STATE_FAILED = 1<<4,
			TASK_STATE_AWAITING = 1<<5,

			METHOD_SLOT_RUN = 10,
		};
		
		typedef InterlockedQueue<Task*> TaskQueue;

	protected:
		TaskContext mContext;
		uint8 mTaskFlags;
		TaskQueue mAwaiting; // are waiting for completion of this

	public:
		TaskContext* getContext();

		// flags

		bool isCompleted() const;
		bool isScheduled() const;
		bool isFailed() const;
		bool isCanceled() const;
		bool isAwaiting() const;

		void markCompleted();
		void markScheduled();
		void markFailed();
		void markCanceled();
		void markAwaiting();

		void unmarkCompleted();
		void unmarkScheduled();
		void unmarkFailed();
		void unmarkCanceled();
		void unmarkAwaiting();

		TaskQueue* getAwaitingQueue();

		// beer methods

		static void BEER_CALL init(Thread* thread, StackRef<Task> receiver, StackRef<Task> ret);

		static void BEER_CALL schedule(Thread* thread, StackRef<Task> receiver);
		static void BEER_CALL await(Thread* thread, StackRef<Task> receiver);

		static void BEER_CALL getScheduled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getCompleted(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getFailed(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getCanceled(Thread* thread, StackRef<Task> receiver, StackRef<Boolean> ret);

		static void BEER_CALL getId(Thread* thread, StackRef<Task> receiver, StackRef<Integer> ret);

		static void BEER_CALL abstractRun(Thread* thread, StackRef<Task> receiver);

		// traversers
		static void TaskInstanceTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance);
	};

	class TaskInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};

	INLINE TaskContext* Task::getContext()
	{
		return &mContext;
	}

	INLINE bool Task::isCompleted() const
	{
		return Object::hasFlag(mTaskFlags, TASK_STATE_COMPLETED);
	}

	INLINE bool Task::isScheduled() const
	{
		return Object::hasFlag(mTaskFlags, TASK_STATE_SCHEDULED);
	}

	INLINE bool Task::isFailed() const
	{
		return Object::hasFlag(mTaskFlags, TASK_STATE_FAILED);
	}

	INLINE bool Task::isCanceled() const
	{
		return Object::hasFlag(mTaskFlags, TASK_STATE_CANCELED);
	}

	INLINE bool Task::isAwaiting() const
	{
		return Object::hasFlag(mTaskFlags, TASK_STATE_AWAITING);
	}

	INLINE void Task::markCompleted()
	{
		Object::markFlag(mTaskFlags, TASK_STATE_COMPLETED);
	}

	INLINE void Task::markScheduled()	
	{
		Object::markFlag(mTaskFlags, TASK_STATE_SCHEDULED);
	}

	INLINE void Task::markFailed()
	{
		Object::markFlag(mTaskFlags, TASK_STATE_FAILED);
	}

	INLINE void Task::markCanceled()
	{
		Object::markFlag(mTaskFlags, TASK_STATE_CANCELED);
	}

	INLINE void Task::markAwaiting()
	{
		Object::markFlag(mTaskFlags, TASK_STATE_AWAITING);
	}

	INLINE void Task::unmarkCompleted()
	{
		Object::unmarkFlag(mTaskFlags, TASK_STATE_COMPLETED);
	}

	INLINE void Task::unmarkScheduled()
	{
		Object::unmarkFlag(mTaskFlags, TASK_STATE_SCHEDULED);
	}

	INLINE void Task::unmarkFailed()
	{
		Object::unmarkFlag(mTaskFlags, TASK_STATE_FAILED);
	}

	INLINE void Task::unmarkCanceled()
	{
		Object::unmarkFlag(mTaskFlags, TASK_STATE_CANCELED);
	}

	INLINE void Task::unmarkAwaiting()
	{
		Object::unmarkFlag(mTaskFlags, TASK_STATE_AWAITING);
	}
	
	INLINE Task::TaskQueue* Task::getAwaitingQueue()
	{
		return &mAwaiting;
	}
};
