#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class String;
	class Class;

	class Param : public Object
	{
	public:
		enum
		{
			PARAM_CHILDREN_COUNT = OBJECT_CHILDREN_COUNT + 2, // type, name

			CHILD_ID_PARAM_TYPE = OBJECT_CHILDREN_COUNT,
			CHILD_ID_PARAM_NAME = OBJECT_CHILDREN_COUNT + 1,
		};

	protected:
		// nothing

	public:
		static void BEER_CALL getParamType(Thread* thread, StackRef<Param> receiver, StackRef<Class> ret);
		static void BEER_CALL setParamType(Thread* thread, StackRef<Param> receiver, StackRef<Class> type);

		static void BEER_CALL getName(Thread* thread, StackRef<Param> receiver, StackRef<String> ret);
		static void BEER_CALL setName(Thread* thread, StackRef<Param> receiver, StackRef<String> name);
	};
};