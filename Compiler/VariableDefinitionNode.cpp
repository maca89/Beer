#include "stdafx.h"

BeerlangCompiler::AST::VariableDefinitionNode::~VariableDefinitionNode()
{
	delete mDeclaration;
	delete mExpression;
}

void BeerlangCompiler::AST::VariableDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}