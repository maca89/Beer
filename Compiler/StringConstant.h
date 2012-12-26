#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class StringConstant : public DescriptorInfo
		{
		protected:

			std::string mValue;
			int16 mUsed;
			
		public:

			INLINE StringConstant(std::string value) : mValue(value), mUsed(0) { }

			INLINE const std::string & getValue() const { return mValue; }
			INLINE int16 getUsed() const { return mUsed; }

			INLINE void incrementUsed() { mUsed++; }
			INLINE void decrementUsed() { mUsed--; }
			
		};
	}
}