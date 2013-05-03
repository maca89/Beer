#pragma once

#include "Node.h"

namespace Beer
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
	}
}

