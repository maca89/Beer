#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class String;
	class Class;

	class Property : public Object
	{
	public:
		enum
		{
			PROPERTY_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 2, // type, name
			
			CHILD_ID_PROPERTY_TYPE = OBJECT_CHILDREN_COUNT,
			CHILD_ID_PROPERTY_NAME = OBJECT_CHILDREN_COUNT + 1,
		};

		static void BEER_CALL getPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> ret);
		static void BEER_CALL setPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> type);

		static void BEER_CALL getName(Thread* thread, StackRef<Property> receiver, StackRef<String> ret);
		static void BEER_CALL setName(Thread* thread, StackRef<Property> receiver, StackRef<String> name);
	};
};