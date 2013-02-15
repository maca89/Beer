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

		INLINE const ClassReflection* getType() const { return mType; }
		INLINE ClassReflection* getType() { return mType; }

		INLINE void setType(ClassReflection* value) { mType = value; }

		// name

		INLINE const char_t* getName() const { return mName->c_str(); }
		INLINE void setName(Reference<String> value) { mName = value; }
	};
};