#pragma once

#include "TraverseObjectReceiver.h"

namespace Beer
{
	class GCObject;
	class Object;

	class ChildrenForwarder : protected TraverseObjectReceiver
	{
	public:

		void forward(GCObject* gcObj);

	protected:

		virtual void traverseObjectPtr(Object** ptrToObject);
	};
};