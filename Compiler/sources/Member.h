#pragma once

#include "prereq.h"
#include "Descriptor.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class Class;
		class StringDescriptor;

		/*
		 * This class stores information about member variables of a class.
		 */
		class Member : public Descriptor
		{
		public:

			typedef uint16 id;
			typedef int8 flags;	
			
			enum Flags
			{
				NoFlags = 0,
			};


		protected:
			Class *				mClass;

			id					mId;
			flags				mFlags;
			Class *				mType;
			StringDescriptor *	mName;
			std::string			mFullName;

		public:

			INLINE Member(Class * cls, id id, flags flags, Class * type, StringDescriptor * name, const std::string & fullName)
				:	mClass(cls),
					mId(id),
					mFlags(flags),
					mType(type),
					mName(name),
					mFullName(fullName)
			{ }

			INLINE Class * getClass()
			{
				return mClass;
			}

			INLINE id getId() const
			{
				return mId;
			}

			INLINE flags getFlags() const
			{
				return mFlags;
			}

			INLINE Class * getType()
			{
				return mType;
			}

			INLINE StringDescriptor * getName()
			{
				return mName;
			}

			INLINE const std::string & getFullName() const
			{
				return mFullName;
			}

			INLINE void setId(id id)
			{
				mId = id;
			}
		};
	}
}