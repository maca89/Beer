#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class BooleanConstantNode : public ConstantNode
		{
		protected:

			bool mValue;

		public:

			INLINE BooleanConstantNode(bool value) : mValue(value) { }

			INLINE bool getValue() const { return mValue; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}