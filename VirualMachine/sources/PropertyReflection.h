#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class String;
	class Class;

	class PropertyReflection : public Object
	{
	public:
		enum
		{
			PROPERTY_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 2 // type, name
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
		INLINE PropertyReflection()
		{
			// never called!
		}

		INLINE ~PropertyReflection()
		{
			// never called!
		}
	};

	/*class PropertyClass
	{
	};*/
};