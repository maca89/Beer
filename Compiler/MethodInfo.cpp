#include "stdafx.h"

/*BeerlangCompiler::SemanticAnalysis::MethodInfo::MethodInfo(ClassInfo * cls, method_id id, method_flags flags)
: mClass(cls), mId(id), mFlags(flags), mName(mClass->getName()), mParameterId(FIRST_PARAMETER_ID)
{
}*/

BeerlangCompiler::SemanticAnalysis::parameter_id BeerlangCompiler::SemanticAnalysis::MethodInfo::getParameterCount() const
{
	parameter_id count = mReturnParameters.size() + mParameters.size();

	if (mBlock)
	{
		count += mBlock->getVariableCount();
	}

	return count;
}

BeerlangCompiler::SemanticAnalysis::MethodParameterInfo * BeerlangCompiler::SemanticAnalysis::MethodInfo::getParameter(std::string name)
{
	MethodParameterMapIterator it = mParameterMap.find(name);

	if (it == mParameterMap.end()) return NULL;

	return it->second;
}

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::MethodInfo::getMethodCall(AST::ExpressionNode * call)
{
	std::map<AST::ExpressionNode *, MethodInfo *>::iterator it = mMethodCalls.find(call);

	if (it == mMethodCalls.end()) return NULL;

	return it->second;
}

BeerlangCompiler::SemanticAnalysis::BlockInfo * BeerlangCompiler::SemanticAnalysis::MethodInfo::setBlock(AST::BlockNode * block)
{
	mBlock = new BlockInfo(block, mParameterId);

	mClass->getProgram()->getDescriptorDictionary()->addDescriptor(mBlock);

	return mBlock;
}

void BeerlangCompiler::SemanticAnalysis::MethodInfo::addMethodCall(AST::ExpressionNode * call, MethodInfo * info)
{
	info->getFullName()->incrementUsed();

	mMethodCalls[call] = info;
}

BeerlangCompiler::SemanticAnalysis::MethodParameterInfo * BeerlangCompiler::SemanticAnalysis::MethodInfo::addParameter(std::string type, std::string name, Output & out)
{
	if (mParameterMap.find(name) != mParameterMap.end())
	{
		out.addError("Multiple parameters " + name + " of method " + mName->getValue() + " class " + mClass->getName()->getValue());

		return NULL;
	}

	StringConstant * nameConstant = NULL;

	if (hasFlags(Native))
	{
		nameConstant = new StringConstant(name);
	}
	else
	{
		nameConstant = mClass->getProgram()->getDescriptorDictionary()->addStringConstant(name);
	}

	MethodParameterInfo * parameter = new MethodParameterInfo(--mParameterId, MethodParameterInfo::NoFlag, mClass->getProgram()->getClass(type), nameConstant);

	if (!hasFlags(Native))
	{
		mClass->getProgram()->getDescriptorDictionary()->addDescriptor(parameter);
	}

	mParameters.push_back(parameter);
	mParameterMap[name] = parameter;

	return parameter;
}

BeerlangCompiler::SemanticAnalysis::MethodParameterInfo * BeerlangCompiler::SemanticAnalysis::MethodInfo::addReturnParameter(std::string type, Output & out)
{
	StringConstant * nameConstant = NULL;

	if (hasFlags(Native))
	{
		nameConstant = new StringConstant(type);
	}
	else
	{
		nameConstant = mClass->getProgram()->getDescriptorDictionary()->addStringConstant(type);
	}

	MethodParameterInfo * parameter = new MethodParameterInfo(--mParameterId, MethodParameterInfo::NoFlag, mClass->getProgram()->getClass(type), nameConstant);

	if (!hasFlags(Native))
	{
		mClass->getProgram()->getDescriptorDictionary()->addDescriptor(parameter);
	}

	mReturnParameters.push_back(parameter);
	mReturnParameterMap[type] = parameter;

	return parameter;
}