#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		enum Operator {
			OperatorAssign,
			OperatorPlusAssign,
			OperatorMinusAssign,
			OperatorMultiplicationAssign,
			OperatorDivisionAssign,

			OperatorPlus,
			OperatorMinus,
			OperatorMultiplication,
			OperatorDivision,

			OperatorIncrement,
			OperatorDecrement,

			OperatorEquals,
			OperatorNotEquals,
			OperatorGreater,
			OperatorGreaterOrEquals,
			OperatorLower,
			OperatorLowerOrEquals,

			OperatorLogicalAnd,
			OperatorLogicalOr,
			OperatorLogicalNegation,

			OperatorBitwiseAnd,
			OperatorBitwiseOr,
			OperatorBitwiseNegation,
			OperatorBitwiseXor			
		};

		class ExpressionNode : public Node { };

		typedef std::list<ExpressionNode *> ExpressionList;
		typedef std::list<ExpressionNode *>::iterator ExpressionListIterator;
		typedef std::list<ExpressionNode *>::reverse_iterator ExpressionListReverseIterator;
	}
}

