#include "stdafx.h"

BeerlangCompiler::CodeGeneration::ByteCodeGenerator::ByteCodeGenerator(std::ostream & out, SemanticAnalysis::MethodInfo * method, SemanticAnalysis::BlockInfo * block, bool reverseBytes)
	: mBw(out, reverseBytes), mInstructionCount(0), mInstructionOffset(0), mInstructionLength(0), mStackSize(0), mMaxStackSize(0)
{
	mMethod = method;
	mClass = method->getClass();
	mProgram = mClass->getProgram();
	mBlock = block;
}

BeerlangCompiler::CodeGeneration::ByteCodeGenerator::ByteCodeGenerator(std::ostream & out, SemanticAnalysis::MethodInfo * method, SemanticAnalysis::BlockInfo * block, count instructionOffset, bool reverseBytes)
	: mBw(out, reverseBytes),  mInstructionCount(0), mInstructionOffset(instructionOffset), mInstructionLength(0), mStackSize(0), mMaxStackSize(0)
{
	mMethod = method;
	mClass = method->getClass();
	mProgram = mClass->getProgram();
	mBlock = block;
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::BlockNode * block)
{
	ASSERT(mBlock, "mBlock is NULL");

	if (mBlock->getBlock() != block)
	{
		mBlock = mBlock->getBlock(block);
	}

	ASSERT(mBlock, "mBlock is NULL");

	if (mBlock->getVariableCount())
	{
		generateMOVE_TOP(mBlock->getVariableCount());
		incrementStack(mBlock->getVariableCount());
	}

	NodeVisitor::visit(block);

	if (!mBlock->isTerminated())
	{
		ASSERT(mBlock != mMethod->getBlock(), "Method block must be terminated with return");

		if (mBlock->getVariableCount())
		{
			generateMOVE_TOP(-mBlock->getVariableCount());
			decrementStack(mBlock->getVariableCount());
		}
	}	

	ASSERT(mStackSize == 0, "Stack size is not zero");
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::VariableDefinitionNode * variable)
{
	NodeVisitor::visit(variable);
	
	SemanticAnalysis::VariableInfo * variableInfo = mBlock->getVariable(variable->getDeclaration()->getName());

	ASSERT(variableInfo, "Variable info is NULL");

	generateSTORE(variableInfo->getId());
	decrementStack();
}


void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::AssignmentNode * assignment)
{
	assignment->getExpression()->accept(this);

	SemanticAnalysis::ExpressionInfo * left = mBlock->getExpression(assignment->getLValue());

	ASSERT(left, "Left side of assignment is NULL");

	switch (left->getKind())
	{
	case SemanticAnalysis::ExpressionInfo::Member:
		generateTOP(0);
		incrementStack();
		generateASSIGN(left->getId());
		decrementStack(2);
		break;

	case SemanticAnalysis::ExpressionInfo::Parameter:
	case SemanticAnalysis::ExpressionInfo::Variable:
		generateSTORE(left->getId());
		decrementStack();
		break;

	default:
		throw SemanticException("Unknown type on left side of assignment");
	}
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::IfNode * ifNode)
{
	std::list<std::stringstream *> outs;

	for (ExpressionBlockMapIterator it = ifNode->getIfBlocks()->begin(); it != ifNode->getIfBlocks()->end(); it++)
	{
		std::stringstream * outFirst = new std::stringstream(std::ios::binary | std::ios::out);
		ByteCodeGenerator bcgFirst(*outFirst, mMethod, mBlock, mInstructionOffset + mInstructionCount + 1, mBw.getReverseBytes());

		it->first->accept(&bcgFirst);
		
		mInstructionCount += bcgFirst.getInstructionCount();
		mInstructionLength += bcgFirst.getInstructionLength();
		incrementStack(bcgFirst.mStackSize);
		decrementStack(bcgFirst.mStackSize);

		std::stringstream * outSecond = new std::stringstream(std::ios::binary | std::ios::out);
		ByteCodeGenerator bcgSecond(*outSecond, mMethod, mBlock, mInstructionOffset + mInstructionCount + 1, mBw.getReverseBytes());
		
		it->second->accept(&bcgSecond);

		mInstructionCount += bcgSecond.getInstructionCount();
		mInstructionLength += bcgSecond.getInstructionLength();
		incrementStack(bcgSecond.mStackSize);
		decrementStack(bcgSecond.mStackSize);

		BinaryWriter bwTemp(*outFirst, mBw.getReverseBytes());

		mInstructionLength += bwTemp.write((instruction)JMP_FALSE);
		mInstructionLength += bwTemp.write((uint16)(mInstructionOffset + mInstructionCount + 1 + 1));
		bwTemp.write(*outSecond);

		outs.push_back(outFirst);
		
		mInstructionCount += 2;

	}

	std::stringstream * elseOut = NULL; 

	if (ifNode->getElseBlock())
	{
		elseOut = new std::stringstream(std::ios::binary | std::ios::out);

		ByteCodeGenerator bcg(*elseOut, mMethod, mBlock->getBlock(ifNode->getElseBlock()), mBw.getReverseBytes());

		ifNode->getElseBlock()->accept(&bcg);

		mInstructionCount += bcg.getInstructionCount();
		mInstructionLength += bcg.getInstructionLength();
		incrementStack(bcg.mStackSize);
		decrementStack(bcg.mStackSize);
	}

	for (std::list<std::stringstream *>::iterator it = outs.begin(); it != outs.end(); it++)
	{
		mBw.write(**it);
		mInstructionLength += mBw.write((instruction)JMP);
		mInstructionLength += mBw.write((uint16)(mInstructionOffset + mInstructionCount));
	}

	if (elseOut)
	{
		mBw.write(*elseOut);
	}
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::WhileNode * whileNode)
{
	count startCount = mInstructionCount;

	whileNode->getCondition()->accept(this);

	std::stringstream out;

	ByteCodeGenerator bcg(out, mMethod, mBlock->getBlock(whileNode->getBody()), mInstructionOffset + mInstructionCount + 1, mBw.getReverseBytes());

	whileNode->getBody()->accept(&bcg);

	mInstructionCount += bcg.getInstructionCount();
	mInstructionLength += bcg.getInstructionLength();
	incrementStack(bcg.mMaxStackSize);
	decrementStack(bcg.mMaxStackSize);

	generateJMP_FALSE(mInstructionOffset + mInstructionCount + 2);
	decrementStack();

	mBw.write(out);

	generateJMP(mInstructionOffset + startCount);
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::ReturnNode * returnNode)
{
	NodeVisitor::visit(returnNode);

	ASSERT(returnNode->getArguments()->size() == mMethod->getReturnParameters()->size(), "Return node parameter count does not equal method return parameter count");

	if (returnNode->getArguments()->size())
	{
		generateSTORE(-static_cast<int16>(mMethod->getParameters()->size() + 1));
		decrementStack();
	}

	generateRETURN(mBlock->getAllVariableCount() + mMethod->getParameters()->size() + 1);
	decrementStack(mBlock->getVariableCount());
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::ConstructorCallNode * call)
{
	SemanticAnalysis::MethodInfo * constructor = mMethod->getMethodCall(call);

	ASSERT(constructor, "Constructor is NULL");

	generateMOVE_TOP(1);
	incrementStack();

	for (AST::ExpressionListReverseIterator it = call->getArguments()->rbegin(); it != call->getArguments()->rend(); it++)
	{
		(*it)->accept(this);
	}

	generateNEW(constructor->getClass()->getId());
	incrementStack();

	generateSPECIAL_INVOKE(constructor->getFullName()->getDescriptorId());
	decrementStack(constructor->getParameters()->size() + 1);
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::MethodCallNode * call)
{	
	SemanticAnalysis::MethodInfo * method = mMethod->getMethodCall(call);

	if (method)
	{
		if (method->getReturnParameters()->size() != 0)
		{
			generateMOVE_TOP(method->getReturnParameters()->size());
			incrementStack(method->getReturnParameters()->size());
		}

		for (AST::ExpressionListReverseIterator it = call->getArguments()->rbegin(); it != call->getArguments()->rend(); it++)
		{
			(*it)->accept(this);
		}

		call->getInstance()->accept(this);

		generateINVOKE(method->getFullName()->getDescriptorId());
		decrementStack(method->getParameters()->size() + 1);

		if (call->getType() == AST::MethodCallNode::Statement)
		{
			if (method->getReturnParameters()->size() != 0)
			{
				generateMOVE_TOP(-static_cast<int16>(method->getReturnParameters()->size()));
				decrementStack(method->getReturnParameters()->size());
			}
		}
	}
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::IdentifierNode * identifier)
{
	SemanticAnalysis::ExpressionInfo * info = mBlock->getExpression(identifier);

	ASSERT(info, "Info is NULL");

	switch (info->getKind())
	{
	case SemanticAnalysis::ExpressionInfo::Member:
		generateTOP(0);
		incrementStack();
		generateLOAD(info->getId());
		break;

	case SemanticAnalysis::ExpressionInfo::Parameter:
	case SemanticAnalysis::ExpressionInfo::Variable:
		generateTOP(info->getId());
		incrementStack();
		break;

	default:
		throw std::exception("");
	}
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::ThisNode * thisNode)
{
	generateTOP(0);
	incrementStack();
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::BooleanConstantNode * constant)
{
	generatePUSH_BOOL(constant->getValue());
	incrementStack();
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::CharacterConstantNode * constant)
{
	generatePUSH_CHAR(constant->getValue());
	incrementStack();
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::IntegerConstantNode * constant)
{
	generatePUSH_INT32(constant->getValue());
	incrementStack();
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::FloatConstantNode * constant)
{
	generatePUSH_FLOAT(constant->getValue());
	incrementStack();
}

void BeerlangCompiler::CodeGeneration::ByteCodeGenerator::visit(AST::StringConstantNode * constant)
{
	SemanticAnalysis::StringConstant * string = mProgram->getDescriptorDictionary()->getStringDescriptor(constant->getValue());

	if (!string) throw std::exception("D");

	generatePUSH_STRING(string->getDescriptorId());
	incrementStack();
}