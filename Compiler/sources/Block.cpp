#include "stdafx.h"

#include "Block.h"

#include "Output.h"

using namespace Beer::SemanticAnalysis;

Block * Block::getBlock(::Beer::AST::BlockNode * block)
{
	std::map<AST::BlockNode *, Block *>::iterator it = mBlocks.find(block);

	if (it == mBlocks.end()) return NULL;

	return it->second; 
}

Variable * Block::getVariable(std::string name)
{
	VariableMap::iterator it = mVariables.find(name);

	if (it == mVariables.end())
	{
		if (mParent)
		{
			return mParent->getVariable(name);
		}
		else
		{
			return NULL;
		}
	}

	return it->second;
}

Expression * Block::getExpression(::Beer::AST::ExpressionNode * expression)
{
	ExpressionMap::iterator it = mExpressions.find(expression);

	if (it == mExpressions.end()) return NULL;

	return it->second;
}

Block * Block::addBlock(::Beer::AST::BlockNode * blockNode)
{
	Block * block = new Block(this, blockNode, mParameterId);

	mBlocks[blockNode] = block;

	return block;
}

Method * Block::getMethodCall(::Beer::AST::ExpressionNode * call)
{
	std::map<AST::ExpressionNode *, Method *>::iterator it = mMethodCalls.find(call);

	if (it == mMethodCalls.end()) return NULL;

	return it->second;
}


Variable * Block::declareVariable(Class * type, const std::string & name, ::Beer::Output & out)
{
	if (mParameterId < 1) mParameterId = 1;
	
	VariableMap::iterator it = mVariables.find(name);

	if (it != mVariables.end())
	{
		out.addError("Multiple variables with name " + name + " in class ");

		return NULL;
	}

	Variable * variable = new Variable(mParameterId++, type, name);

	mVariables[name] = variable;

	if (mParent)
	{
		Variable * Variable = mParent->getVariable(name);

		if (Variable)
		{
			out.addWarning("Variable " + name + " is shadowed");
		}
	}

	return variable;
}

Variable * Block::defineVariable(Class * type, const std::string & name, ::Beer::Output & out)
{
	if (mParameterId < 1) mParameterId = 1;

	VariableMap::iterator it = mVariables.find(name);

	if (it != mVariables.end())
	{
		out.addError("Multiple variables with name " + name + " in class ");

		return NULL;
	}

	Variable * variable = new Variable(mParameterId++, type, name, true);

	mVariables[name] = variable;

	return variable;
}

void Block::assignVariable(const std::string & name)
{
	VariableMap::iterator it = mVariables.find(name);

	if (it != mVariables.end())
	{
		it->second->isDefined(true);
	}
}

void Block::addMethodCall(::Beer::AST::ExpressionNode * call, Method * method)
{
	mMethodCalls[call] = method;
}

void Block::addExpression(::Beer::AST::ExpressionNode * expression, Expression * expr)
{
	mExpressions[expression] = expr;
}