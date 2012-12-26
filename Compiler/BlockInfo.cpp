#include "stdafx.h"

BeerlangCompiler::SemanticAnalysis::BlockInfo * BeerlangCompiler::SemanticAnalysis::BlockInfo::getBlock(AST::BlockNode * block)
{
	std::map<AST::BlockNode *, BlockInfo *>::iterator it = mBlocks.find(block);

	if (it == mBlocks.end()) return NULL;

	return it->second; 
}

BeerlangCompiler::SemanticAnalysis::VariableInfo * BeerlangCompiler::SemanticAnalysis::BlockInfo::getVariable(std::string name)
{
	VariableMapIterator it = mVariables.find(name);

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

BeerlangCompiler::SemanticAnalysis::ExpressionInfo * BeerlangCompiler::SemanticAnalysis::BlockInfo::getExpression(AST::ExpressionNode * expression)
{
	ExpressionMapIterator it = mExpressions.find(expression);

	if (it == mExpressions.end()) return NULL;

	return it->second;
}

BeerlangCompiler::SemanticAnalysis::BlockInfo * BeerlangCompiler::SemanticAnalysis::BlockInfo::addBlock(AST::BlockNode * block)
{
	BlockInfo * blockInfo = new BlockInfo(this, block, mParameterId);

	mBlocks[block] = blockInfo;

	return blockInfo;
}

BeerlangCompiler::SemanticAnalysis::VariableInfo * BeerlangCompiler::SemanticAnalysis::BlockInfo::declareVariable(ClassInfo * type, std::string name, Output & out)
{
	if (mParameterId < 1) mParameterId = 1;
	
	VariableMapIterator it = mVariables.find(name);

	if (it != mVariables.end())
	{
		out.addError("Multiple variables with name " + name + " in class ");

		return NULL;
	}

	VariableInfo * variable = new VariableInfo(mParameterId++, type, name);

	mVariables[name] = variable;

	if (mParent)
	{
		VariableInfo * variableInfo = mParent->getVariable(name);

		if (variableInfo)
		{
			out.addWarning("Variable " + name + " is shadowed");
		}
	}

	return variable;
}

BeerlangCompiler::SemanticAnalysis::VariableInfo * BeerlangCompiler::SemanticAnalysis::BlockInfo::defineVariable(ClassInfo * type, std::string name, Output & out)
{
	if (mParameterId < 1) mParameterId = 1;

	VariableMapIterator it = mVariables.find(name);

	if (it != mVariables.end())
	{
		out.addError("Multiple variables with name " + name + " in class ");

		return NULL;
	}

	VariableInfo * variable = new VariableInfo(mParameterId++, type, name, true);

	mVariables[name] = variable;

	return variable;
}

void BeerlangCompiler::SemanticAnalysis::BlockInfo::assignVariable(std::string name)
{
	VariableMapIterator it = mVariables.find(name);

	if (it != mVariables.end())
	{
		it->second->isDefined(true);
	}
}

void BeerlangCompiler::SemanticAnalysis::BlockInfo::addExpression(AST::ExpressionNode * expression, ExpressionInfo * expressionInfo)
{
	mExpressions[expression] = expressionInfo;
}