#include "stdafx.h"

#include "VariableDefinitionNode.h"

#include "VariableDeclarationNode.h"
#include "ExpressionNode.h"
#include "NodeVisitor.h"

using namespace Beer::AST;

VariableDefinitionNode::~VariableDefinitionNode()
{
	delete mDeclaration;
	delete mExpression;
}

void VariableDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}