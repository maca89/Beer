#include "stdafx.h"

BeerlangCompiler::AST::MethodDefinitionNode::MethodDefinitionNode(std::string name, BlockNode * body)
	: mName(name), mBody(body)
{
	mReturnParameters = new StringList;

	mParameters = new MethodParameterList;
}

BeerlangCompiler::AST::MethodDefinitionNode::MethodDefinitionNode(std::string name, MethodParameterList * parameters, BlockNode * body)
	: mName(name), mParameters(parameters), mBody(body)
{
	mReturnParameters = new StringList;
}

BeerlangCompiler::AST::MethodDefinitionNode::MethodDefinitionNode(std::string returnParameter, std::string name, BlockNode * body)
	: mName(name), mBody(body)
{
	mReturnParameters = new StringList;

	mReturnParameters->push_back(returnParameter);

	mParameters = new MethodParameterList;
}

BeerlangCompiler::AST::MethodDefinitionNode::MethodDefinitionNode(StringList * returnParameters, std::string name, BlockNode * body)
	: mReturnParameters(returnParameters), mName(name), mBody(body)
{
	mParameters = new MethodParameterList;
}

BeerlangCompiler::AST::MethodDefinitionNode::MethodDefinitionNode(std::string returnParameter, std::string name, MethodParameterNode * parameter, BlockNode * body)
	: mName(name), mBody(body)
{
	mReturnParameters = new StringList;

	mReturnParameters->push_back(returnParameter);

	mParameters = new MethodParameterList;

	mParameters->push_back(parameter);
}

BeerlangCompiler::AST::MethodDefinitionNode::~MethodDefinitionNode()
{
	delete mReturnParameters;

	for (MethodParameterListIterator it = mParameters->begin(); it != mParameters->end(); it++)
	{
		delete *it;
	}

	delete mParameters;

	delete mBody;
}

void BeerlangCompiler::AST::MethodDefinitionNode::accept(NodeVisitor * visitor)
{
	visitor->visit(this);
}