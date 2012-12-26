#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class VariableDefinitionNode : public StatementNode
		{
		protected:

			VariableDeclarationNode * mDeclaration;
			ExpressionNode * mExpression;
	
		public:

			INLINE VariableDefinitionNode(VariableDeclarationNode * declaration, ExpressionNode * expression) : mDeclaration(declaration), mExpression(expression) { }
			virtual ~VariableDefinitionNode();

			INLINE VariableDeclarationNode * getDeclaration() { return mDeclaration; }	
			INLINE ExpressionNode * getExpression() const { return mExpression; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}