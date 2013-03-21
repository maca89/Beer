#pragma once
#include "prereq.h"
#include "TraverseObjectReceiver.h"


namespace Beer
{
	class Object;
	class Thread;

	class ObjectInspector : public TraverseObjectReceiver
	{
	public:
		typedef std::list<Object*> ChildrenList;

	protected:
		Thread* mThread;
		Object* mObject;
		uint32 mLevel;
		uint32 mMaxLevel;
		ChildrenList mChildren;
		

	public:
		ObjectInspector(uint32 level = 0);
		~ObjectInspector();

		void start(Thread* thread, Object* object);

		void nextChild();
		bool hasChild();
		Object* getChild();
		//Object** getChildPtr();

		void debugPrintObject();

		static void debugPrintObject(Thread* thread, Object* object);

		// interface TraverseObjectReceiver
		virtual void traverseObjectPtr(Object** ptrToObject);

	protected:
		void printPadding();
	};
};