#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class IntegerConstantNode : public ConstantNode
		{
		protected:

			int mValue;

		public:

			INLINE IntegerConstantNode(int value) : mValue(value) { }

			INLINE int getValue() const { return mValue; }

			virtual void accept(NodeVisitor * visitor);
		};
	}
}