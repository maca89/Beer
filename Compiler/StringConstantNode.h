#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class StringConstantNode : public ConstantNode
		{
		protected:

			std::string mValue;

		public:

			INLINE StringConstantNode(std::string value) : mValue(value) { }

			INLINE std::string getValue() const { return mValue; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}