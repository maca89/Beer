#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class String;
	class Class;

	class ParamReflection : public Object
	{
	public:
		enum
		{
			PARAM_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 2 // type, name
		};

	protected:
		// nothing

	public:
		// type
		Class* getType();
		void setType(Class* value);

		// name

		String* getName();
		void setName(String* value);

	private:
		INLINE ParamReflection()
		{
			// never called!
		}

		INLINE ~ParamReflection()
		{
			// never called!
		}
	};

	/*class ParamClass
	{
	};*/
};