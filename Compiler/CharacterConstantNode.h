#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class CharacterConstantNode : public ConstantNode
		{
		protected:

			char mValue;

		public:

			INLINE CharacterConstantNode(char value) : mValue(value) { }

			INLINE char getValue() const { return mValue; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}