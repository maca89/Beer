#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class ClassReflection;

	class PropertyReflection : public Object
	{
	protected:
		ClassReflection* mType;
		std::string mName; // TODO

	public:

		// type

		const ClassReflection* getType() const { return mType; }
		ClassReflection* getType() { return mType; }

		void setType(ClassReflection* value) { mType = value; }

		// name

		const char* getName() const { return mName.c_str(); }
		void setName(std::string value) { mName = value; }
	};
};