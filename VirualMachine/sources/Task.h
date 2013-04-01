#pragma once
#include "prereq.h"
#include "Object.h"
#include "Boolean.h"
#include "Integer.h"
#include "TaskContext.h"


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

			TASK_STATE_SCHEDULED = 0x01,
			TASK_STATE_COMPLETED = 0x02,
			TASK_STATE_CANCELED = 0x04,
			TASK_STATE_FAILED = 0x08,
		};

	protected:
		TaskContext mContext;
		uint8 mTaskFlags;

	public:
		TaskContext* getContext();

		// flags

		bool isCompleted() const;
		bool isScheduled() const;
		bool isFailed() const;
		bool isCanceled() const;

		void markCompleted();
		void markScheduled();
		void markFailed();
		void markCanceled();

		void unmarkCompleted();
		void unmarkScheduled();
		void unmarkFailed();
		void unmarkCanceled();

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
};
