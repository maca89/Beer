#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class ThisNode : public ExpressionNode
		{
			virtual void accept(NodeVisitor * visitor);
		};
	}
}