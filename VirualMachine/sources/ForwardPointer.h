#pragma once
#include "Object.h"

namespace Beer
{
	class ForwardPointer : public Object
	{
	public:

		INLINE Object* getObject()
		{
			return NULL; // TODO
			//return mIdentity;
			//return mChildren[1];
		}
	};
}