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
		* This class represents method parameter - passed parameters or return parameters.
		*/
		class MethodParameter : public Descriptor
		{
		public:

			typedef int16 id;
			typedef int8 flags;

			enum Flag
			{
				NoFlag
			};

		protected:

			id					mId;
			flags				mFlags;
			Class *				mType;
			StringDescriptor *	mName;

		public:

			INLINE MethodParameter(id id, flags flags, Class * type, StringDescriptor * name)
				:	mId(id),
					mFlags(flags),
					mType(type),
					mName(name)
			{ }


			virtual ~MethodParameter()
			{ }


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

			INLINE StringDescriptor * getName() const
			{
				return mName;
			}
		};
	}
}