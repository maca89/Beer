#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class ClassReflection;

	class ParamReflection : public Object
	{
	public:
		////////////////////////////////////////////////////////////
		////             Initialized by ClassLoader             ////
		////////////////////////////////////////////////////////////
		uint16 mNameCount;
		char* mName;
		
		ClassReflection* mType;
		////////////////////////////////////////////////////////////

	public:
		INLINE ParamReflection()
		{
		}

		INLINE ~ParamReflection()
		{
			// is never called!
		}

		// type
		
		INLINE ClassReflection* getType() { return mType; }
		INLINE void setType(ClassReflection* klass) { mType = klass; }

		// name

		INLINE const char* getName() const { return mName; }
	};
};