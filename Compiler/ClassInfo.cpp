#include "stdafx.h"

BeerlangCompiler::SemanticAnalysis::ClassInfo::ClassInfo(ProgramInfo * program, StringConstant * name, class_flags flags)
	: mProgram(program), mId(0), mFlags(flags), mName(name), mMemberId(FIRST_MEMBER_ID), mMethodId(FIRST_FUNCTION_ID), mContainsConstructor(false), mUsed(false)
{
}

BeerlangCompiler::SemanticAnalysis::MemberInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::getMember(std::string name)
{
	MemberMapIterator it = mMemberMap.find(name);

	if (it == mMemberMap.end()) return NULL;

	return it->second;
}

/*BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::getConstructor(std::vector<ClassInfo *> * arguments)
{
for (MethodListIterator it = mConstructors.begin(); it != mConstructors.end(); it++)
{
if ((*it)->matchArguments(arguments))
{
return *it;
}
}

return NULL;
}*/

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::getMethod(std::string fullName)
{
	MethodMapIterator it = mMethodMap.find(fullName);

	if (it == mMethodMap.end()) return NULL;

	return it->second;
}

/*BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::getOperator(int op)
{
OperatorMapIterator it = mOperators.find(op);

if (it == mOperators.end()) return NULL;

if (!it->second->emtpyParameters()) return NULL;

return it->second;
}

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::getOperator(int op, ClassInfo * parameter)
{
OperatorMapIterator it = mOperators.find(op);

if (it == mOperators.end()) return NULL;

if (!it->second->matchArgument(parameter)) return NULL;

return it->second;
}*/

BeerlangCompiler::SemanticAnalysis::MemberInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::addMember(member_flags flags, std::string type, std::string name, Output & out)
{
	MemberMapIterator it = mMemberMap.find(name);

	if (it != mMemberMap.end())
	{
		out.addError("Multiple member variables with name " + name + " in class " + mName->getValue());

		return NULL;
	}

	StringConstant * nameConstant = NULL;

	if (hasFlags(ClassFlag::Native))
	{
		nameConstant = new StringConstant(name);
	}
	else
	{
		nameConstant = mProgram->getDescriptorDictionary()->addStringConstant(name);
	}

	MemberInfo * member = new MemberInfo(this, mMemberId++, flags, mProgram->getClass(type), nameConstant, mName->getValue() + "::" + name);

	mMembers.push_back(member);
	mMemberMap[name] = member;

	if (!hasFlags(ClassFlag::Native))
	{
		mProgram->getDescriptorDictionary()->addDescriptor(member);
	}

	return member;
}

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::addConstructor(AST::MethodParameterList * parameters, Output & out)
{
	DescriptorDictionary * dictionary = mProgram->getDescriptorDictionary();

	std::string fullName = createMethodFullName(mName->getValue(), parameters);

	if (mMethodMap.find(fullName) != mMethodMap.end())
	{
		out.addError("Constructor " + fullName + " is already defined");

		return NULL;
	}

	method_flags flags = MethodInfo::NoFlag;

	StringConstant * nameConstant = NULL;
	StringConstant * fullNameConstant = NULL;

	if (hasFlags(ClassFlag::Native))
	{
		flags |= MethodInfo::Native;
		nameConstant = new StringConstant(mName->getValue());
		fullNameConstant = new StringConstant(fullName);
	}
	else
	{
		nameConstant = dictionary->addStringConstant(mName->getValue());
		fullNameConstant = dictionary->addStringConstant(fullName);
	}

	mName->incrementUsed();

	MethodInfo * constructor = new MethodInfo(this, mMethodId++, flags, nameConstant, fullNameConstant, MethodInfo::Constructor);

	if (!hasFlags(ClassFlag::Native))
	{
		mProgram->getDescriptorDictionary()->addDescriptor(constructor);
	}

	for (AST::MethodParameterListIterator it = parameters->begin(); it != parameters->end(); it++)
	{
		constructor->addParameter((*it)->getType(), (*it)->getName(), out);
	}

	constructor->addReturnParameter(mName->getValue(), out);

	mContainsConstructor = true;

	mMethods.push_back(constructor);
	mMethodMap[fullName] = constructor;

	return constructor;
}

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::addConversion(std::string type, Output & out)
{
	DescriptorDictionary * dictionary = mProgram->getDescriptorDictionary();

	std::string fullName = createMethodFullName(type, NULL);

	if (mMethodMap.find(fullName) != mMethodMap.end()) 
	{
		out.addError("Multiple conversion methods of type " + mName->getValue() + " to type " + type);

		return NULL;
	}

	method_flags flags = MethodInfo::NoFlag;

	StringConstant * nameConstant = NULL;
	StringConstant * fullNameConstant = NULL;

	if (hasFlags(ClassFlag::Native))
	{
		flags |= MethodInfo::Native;
		nameConstant = new StringConstant(type);
		fullNameConstant = new StringConstant(fullName);
	}
	else
	{
		nameConstant = dictionary->addStringConstant(type);
		fullNameConstant = dictionary->addStringConstant(fullName);
	}

	MethodInfo * conversion = new MethodInfo(this, mMethodId++, flags, nameConstant, fullNameConstant, MethodInfo::Conversion);

	if (!hasFlags(ClassFlag::Native))
	{
		mProgram->getDescriptorDictionary()->addDescriptor(conversion);
	}

	conversion->addReturnParameter(type, out);

	mMethods.push_back(conversion);
	mMethodMap[fullName] = conversion;	

	return conversion;
}

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::addMethod(std::string name, AST::MethodParameterList * parameters, Output & out)
{
	DescriptorDictionary * dictionary = mProgram->getDescriptorDictionary();

	std::string fullName = createMethodFullName(name, parameters);

	if (mMethodMap.find(fullName) != mMethodMap.end()) 
	{
		out.addError("Method " + fullName + " is already defined");

		return NULL;
	}

	method_flags flags = MethodInfo::NoFlag;

	StringConstant * nameConstant = NULL;
	StringConstant * fullNameConstant = NULL;

	if (hasFlags(ClassFlag::Native))
	{
		flags |= MethodInfo::Native;
		nameConstant = new StringConstant(name);
		fullNameConstant = new StringConstant(fullName);
	}
	else
	{
		nameConstant = dictionary->addStringConstant(name);
		fullNameConstant = dictionary->addStringConstant(fullName);
	}

	MethodInfo * method = new MethodInfo(this, mMethodId++, flags, nameConstant, fullNameConstant, MethodInfo::Method);	

	if (!hasFlags(ClassFlag::Native))
	{
		mProgram->getDescriptorDictionary()->addDescriptor(method);
	}

	for (AST::MethodParameterListIterator it = parameters->begin(); it != parameters->end(); it++)
	{
		method->addParameter((*it)->getType(), (*it)->getName(), out);
	}

	mMethods.push_back(method);
	mMethodMap[fullName] = method;	

	return method;
}

BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::addMethod(AST::StringList * returnParameters, std::string name, AST::MethodParameterList * parameters, Output & out)
{
	DescriptorDictionary * dictionary = mProgram->getDescriptorDictionary();

	std::string fullName = createMethodFullName(name, parameters);

	if (mMethodMap.find(fullName) != mMethodMap.end()) 
	{
		out.addError("Method " + fullName + " is already defined");

		return NULL;
	}

	method_flags flags = MethodInfo::NoFlag;

	StringConstant * nameConstant = NULL;
	StringConstant * fullNameConstant = NULL;

	if (hasFlags(ClassFlag::Native))
	{
		flags |= MethodInfo::Native;
		nameConstant = new StringConstant(name);
		fullNameConstant = new StringConstant(fullName);
	}
	else
	{
		nameConstant = dictionary->addStringConstant(name);
		fullNameConstant = dictionary->addStringConstant(fullName);
	}

	MethodInfo * method = new MethodInfo(this, mMethodId++, flags, nameConstant, fullNameConstant, MethodInfo::Method);	

	if (!hasFlags(ClassFlag::Native))
	{
		mProgram->getDescriptorDictionary()->addDescriptor(method);
	}

	for (AST::MethodParameterListIterator it = parameters->begin(); it != parameters->end(); it++)
	{
		method->addParameter((*it)->getType(), (*it)->getName(), out);
	}

	for (AST::StringListIterator it = returnParameters->begin(); it != returnParameters->end(); it++)
	{
		method->addReturnParameter(*it, out);
	}

	mMethods.push_back(method);
	mMethodMap[fullName] = method;	

	return method;
}

/*BeerlangCompiler::SemanticAnalysis::MethodInfo * BeerlangCompiler::SemanticAnalysis::ClassInfo::addOperator(std::string type, int op, AST::MethodParameters * parameters, Output * out)
{
std::string fullName = createMethodFullName(op, parameters);

if (mMethodMap.find(fullName) != mMethodMap.end())
{
out->addError("Operator " + fullName + " is already defined");

return NULL;
}

MethodInfo * method = new MethodInfo(this, mMethodId++, MethodInfo::NoFlag, SemanticAnalyser::getOperatorString(op), mProgram->getDescriptorDictionary()->addStringConstant(fullName));

mProgram->getDescriptorDictionary()->addDescriptor(method);

for (AST::MethodParametersIterator it = parameters->begin(); it != parameters->end(); it++)
{
method->addParameter((*it)->getType(), (*it)->getName(), out);
}

method->addReturnParameter(type, out);

mMethods.push_back(method);
mMethodMap[fullName] = method;

return method;
}*/

std::string BeerlangCompiler::SemanticAnalysis::ClassInfo::createMethodFullName(std::string methodName, AST::MethodParameterList * parameters)
{
	std::string fullName = mName->getValue() + "::" + methodName + "(";

	if (parameters)
	{
		AST::MethodParameterListIterator it = parameters->begin();

		if (it != parameters->end())
		{
			fullName += (*it)->getType();
			it++;
		}

		for (; it != parameters->end(); it++)
		{
			fullName += "," + (*it)->getType(); 
		}
	}

	fullName += ")";

	return fullName;
}

std::string BeerlangCompiler::SemanticAnalysis::ClassInfo::createMethodFullName(std::string methodName, AST::ExpressionList * arguments, BlockInfo * block)
{
	std::string fullName = mName->getValue() + "::" + methodName + "(";

	ExpressionInfo * type = NULL;

	AST::ExpressionListIterator it = arguments->begin();

	if (it != arguments->end())
	{
		type = block->getExpression(*it);

		if (type && type->getType())
		{
			fullName += type->getType()->getName()->getValue();
		}

		it++;
	}

	for (; it != arguments->end(); it++)
	{
		type = block->getExpression(*it);

		if (type && type->getType())
		{
			fullName += "," + type->getType()->getName()->getValue();
		}
	}

	fullName += ")";

	return fullName;
}

/*std::string BeerlangCompiler::SemanticAnalysis::ClassInfo::createMethodFullName(int op)
{
return mName->getValue() + "::" + SemanticAnalyser::getOperatorString(op) + "()";
}

std::string BeerlangCompiler::SemanticAnalysis::ClassInfo::createMethodFullName(int op, AST::MethodParameterNode * rightOperand)
{
return mName->getValue() + "::" + SemanticAnalyser::getOperatorString(op) + "(" + rightOperand->getType() + ")";
}

std::string BeerlangCompiler::SemanticAnalysis::ClassInfo::createMethodFullName(int op, AST::ExpressionNode * rightOperand, BlockInfo * block)
{
std::string fullName = mName->getValue() + "::" + SemanticAnalyser::getOperatorString(op) + "(";

ClassInfo * type = block->getExpressionResult(rightOperand);

if (type)
{
fullName += type->getName()->getValue();
}

fullName += ")";

return fullName;
}

std::string BeerlangCompiler::SemanticAnalysis::ClassInfo::createMethodFullName(int op, AST::MethodParameters * operands)
{
std::string fullName = mName->getValue() + "::" + SemanticAnalyser::getOperatorString(op) + "(";

if (operands)
{
AST::MethodParametersIterator it = operands->begin();

if (it != operands->end())
{
it++;
}

for (; it != operands->end(); it++)
{
fullName += (*it)->getType(); 
}

for (; it != operands->end(); it++)
{
fullName += "," + (*it)->getType(); 
}
}

fullName += ")";

return fullName;
}*/