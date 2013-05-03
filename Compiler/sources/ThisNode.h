#pragma once

#include "ExpressionNode.h"

namespace Beer
{
	namespace AST
	{
		class ThisNode : public ExpressionNode
		{
			virtual void accept(NodeVisitor * visitor);
		};
	}
}