#pragma once
#include "prereq.h"
#include "StackRef.h"


namespace Beer
{
	class TraverseObjectReceiver
	{
	public:
		virtual ~TraverseObjectReceiver() {}

		//virtual void traverseObject(Object* object) = 0;
		virtual void traverseObjectPtr(Object** ptrToObject) = 0;
		//virtual void traverseObjectRef(StackRef<Object> object) = 0;
	};
};