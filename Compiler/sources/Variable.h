#pragma once

#include "prereq.h"
#include "MethodParameter.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class Variable
		{
		public:

			typedef int16 id;

		protected:

			id		mId;
			Class *					mType;
			std::string				mName;

			bool					mDefined;

		public:

			INLINE Variable(id id, Class * type, std::string name, bool defined = false)
				:	mId(id),
					mType(type),
					mName(name),
					mDefined(defined)
			{ }


			INLINE id getId() const
			{
				return mId;
			}


			INLINE Class * getType()
			{
				return mType;
			}


			INLINE const std::string & getName() const
			{
				return mName;
			}


			INLINE bool isDefined() const
			{
				return mDefined;
			}


			INLINE void isDefined(bool defined)
			{
				mDefined = defined;
			}
		};
	}
}