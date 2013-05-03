#pragma once

#include "prereq.h"
#include "Descriptor.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class StringDescriptor : public Descriptor
		{
		protected:

			std::string	mValue;
			
		public:

			INLINE StringDescriptor(const std::string & value)
				:	mValue(value)
			{ }

			INLINE const std::string & getValue() const
			{
				return mValue;
			}
		};
	}
}