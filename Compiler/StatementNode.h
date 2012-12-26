#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class StatementNode : public Node { };

		typedef std::list<StatementNode *> Statements;
		typedef std::list<StatementNode *>::iterator StatementsIterator;
	}
}

