#pragma once
#include "prereq.h"
#include "Object.h"
#include "StringClass.h"


namespace Beer
{
	class ClassReflection;
	//class String;

	class PropertyReflection : public Object
	{
	protected:
		ClassReflection* mType;
		Reference<String> mName;

	public:

		// type

		const ClassReflection* getType() const { return mType; }
		ClassReflection* getType() { return mType; }

		void setType(ClassReflection* value) { mType = value; }

		// name

		const char_t* getName() const { return mName->c_str(); }
		void setName(Reference<String> value) { mName = value; }
	};
};