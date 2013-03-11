#pragma once
#include "Object.h"

namespace Beer
{
	class ForwardPointer : public Object
	{
	public:

		INLINE Object* getObject()
		{
			return mChildren[1];
		}
	};
}