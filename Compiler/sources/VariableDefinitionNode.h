#pragma once

#include "prereq.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		class VariableDeclarationNode;
		class ExpressionNode;

		class VariableDefinitionNode : public StatementNode
		{
		protected:

			VariableDeclarationNode * mDeclaration;
			ExpressionNode * mExpression;
	
		public:

			INLINE VariableDefinitionNode(VariableDeclarationNode * declaration, ExpressionNode * expression)
				:	mDeclaration(declaration),
					mExpression(expression)
			{ }


			virtual ~VariableDefinitionNode();


			INLINE VariableDeclarationNode * getDeclaration()
			{
				return mDeclaration;
			}

			INLINE ExpressionNode * getExpression()
			{
				return mExpression;
			}


			virtual void accept(NodeVisitor * visitor);

		};
	}
}