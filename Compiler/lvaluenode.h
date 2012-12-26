#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class LValueNode : public ExpressionNode { };

		typedef std::list<LValueNode *> LValueList;
		typedef std::list<LValueNode *>::iterator LValueListIterator;
	}
}