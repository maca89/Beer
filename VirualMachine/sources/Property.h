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
			PROPERTY_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT,
			PROPERTY_METHODS_COUNT = OBJECT_METHODS_COUNT + 4,
		};

	protected:
		Class* mPropertyType;
		String* mPropertyName;

	public:

		Class* getPropertyType();
		void setPropertyType(Class* value);

		String* getPropertyName();
		void setPropertyName(String* value);

		static void BEER_CALL getPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> ret);
		static void BEER_CALL setPropertyType(Thread* thread, StackRef<Property> receiver, StackRef<Class> type);

		static void BEER_CALL getPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> ret);
		static void BEER_CALL setPropertyName(Thread* thread, StackRef<Property> receiver, StackRef<String> name);

		// traversers
		static void PropertyTraverser(TraverseObjectReceiver* receiver, Class* klass, Object* instance);
	};

	// inlined methods
	
	INLINE Class* Property::getPropertyType()
	{
		return mPropertyType;
	}

	INLINE void Property::setPropertyType(Class* value)
	{
		mPropertyType = value;
	}

	INLINE String* Property::getPropertyName()
	{
		return mPropertyName;
	}

	INLINE void Property::setPropertyName(String* value)
	{
		mPropertyName = value;
	}
};