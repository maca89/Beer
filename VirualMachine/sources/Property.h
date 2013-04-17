#pragma once
#include "prereq.h"
#include "Object.h"
#include "String.h"
#include "StackRef.h"


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

		Class* getPropertyType();
		void setPropertyType(Class* value);

		String* getPropertyName();
		void setPropertyName(String* value);

		static void BEER_CALL getPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> ret);
		static void BEER_CALL setPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> type);

		static void BEER_CALL getPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> ret);
		static void BEER_CALL setPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> name);
	};

	// inlined methods
	
	
	
};