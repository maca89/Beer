#include "stdafx.h"

#include "ByteCodeGenerator.h"

#include "AssignmentNode.h"
#include "Block.h"
#include "BlockNode.h"
#include "BooleanConstantNode.h"
#include "CharacterConstantNode.h"
#include "Class.h"
#include "ConstructorCallNode.h"
#include "Expression.h"
#include "FloatConstantNode.h"
#include "IdentifierNode.h"
#include "IfNode.h"
#include "IntegerConstantNode.h"
#include "Method.h"
#include "MethodCallNode.h"
#include "Program.h"
#include "ReturnNode.h"
#include "SemanticException.h"
#include "StringConstantNode.h"
#include "StringDescriptor.h"
#include "Variable.h"
#include "VariableDeclarationNode.h"
#include "VariableDefinitionNode.h"
#include "WhileNode.h"

using namespace Beer::CodeGeneration;

ByteCodeGenerator::ByteCodeGenerator(std::ostream & out, ::Beer::SemanticAnalysis::Method * method, ::Beer::SemanticAnalysis::Block * block, bool reverseBytes)
	:	mBw(out, reverseBytes),
		mInstructionCount(0),
		mInstructionOffset(0),
		mInstructionLength(0),
		mStackSize(0),
		mMaxStackSize(0)
{
	mMethod = method;
	mClass = method->getDeclarator();
	mProgram = mClass->getProgram();
	mBlock = block;
}

ByteCodeGenerator::ByteCodeGenerator(std::ostream & out, ::Beer::SemanticAnalysis::Method * method, ::Beer::SemanticAnalysis::Block * block, count instructionOffset, bool reverseBytes)
	:	mBw(out, reverseBytes),
		mInstructionCount(0),
		mInstructionOffset(instructionOffset),
		mInstructionLength(0),
		mStackSize(0),
		mMaxStackSize(0)
{
	mMethod = method;
	mClass = method->getDeclarator();
	mProgram = mClass->getProgram();
	mBlock = block;
}

void ByteCodeGenerator::visit(::Beer::AST::BlockNode * block)
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
		//incStack(mBlock->getVariableCount());
	}

	NodeVisitor::visit(block);

	if (!mBlock->isTerminated())
	{
		//ASSERT(mBlock != mMethod->getBlock(), "Method block must be terminated with return");

		if (mBlock->getVariableCount())
		{
			generateMOVE_TOP(-mBlock->getVariableCount());
			//decStack(mBlock->getVariableCount());
		}
	}	

	ASSERT(mStackSize == 0, "Stack size is not zero");
}

void ByteCodeGenerator::visit(::Beer::AST::VariableDefinitionNode * variable)
{
	NodeVisitor::visit(variable);
	
	SemanticAnalysis::Variable * var = mBlock->getVariable(variable->getDeclaration()->getName());

	ASSERT(var, "Variable info is NULL");

	generateSTORE(var->getId());
	//decStack();
}


void ByteCodeGenerator::visit(::Beer::AST::AssignmentNode * assignment)
{
	assignment->getExpression()->accept(this);

	SemanticAnalysis::Expression * left = mBlock->getExpression(assignment->getLValue());

	ASSERT(left, "Left side of assignment is NULL");

	switch (left->getKind())
	{
	case SemanticAnalysis::Expression::Member:
		generateTOP(0);
		//incStack();
		generateASSIGN(left->getId());
		//decStack(2);
		break;

	case SemanticAnalysis::Expression::Parameter:
	case SemanticAnalysis::Expression::Variable:
		generateSTORE(left->getId());
		//decStack();
		break;

	default:
		throw SemanticException("Unknown type on left side of assignment");
	}
}

void ByteCodeGenerator::visit(::Beer::AST::IfNode * ifNode)
{
	std::list<std::stringstream *> outs;

	for (AST::ExpressionBlockMap::iterator it = ifNode->getIfBlocks()->begin(); it != ifNode->getIfBlocks()->end(); it++)
	{
		std::stringstream * outFirst = new std::stringstream(std::ios::binary | std::ios::out);
		ByteCodeGenerator bcgFirst(*outFirst, mMethod, mBlock, mInstructionOffset + mInstructionCount + 1, mBw.getReverseBytes());

		it->first->accept(&bcgFirst);
		
		mInstructionCount += bcgFirst.getInstructionCount();
		mInstructionLength += bcgFirst.getInstructionLength();
		incStack(bcgFirst.mStackSize);
		decStack(bcgFirst.mStackSize);

		std::stringstream * outSecond = new std::stringstream(std::ios::binary | std::ios::out);
		ByteCodeGenerator bcgSecond(*outSecond, mMethod, mBlock, mInstructionOffset + mInstructionCount + 1, mBw.getReverseBytes());
		
		it->second->accept(&bcgSecond);

		mInstructionCount += bcgSecond.getInstructionCount();
		mInstructionLength += bcgSecond.getInstructionLength();
		incStack(bcgSecond.mStackSize);
		decStack(bcgSecond.mStackSize);

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
		incStack(bcg.mStackSize);
		decStack(bcg.mStackSize);
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

void ByteCodeGenerator::visit(::Beer::AST::WhileNode * whileNode)
{
	count startCount = mInstructionCount;

	whileNode->getCondition()->accept(this);

	std::stringstream out;

	ByteCodeGenerator bcg(out, mMethod, mBlock->getBlock(whileNode->getBody()), mInstructionOffset + mInstructionCount + 1, mBw.getReverseBytes());

	whileNode->getBody()->accept(&bcg);

	mInstructionCount += bcg.getInstructionCount();
	mInstructionLength += bcg.getInstructionLength();
	//incStack(bcg.mMaxStackSize);
	//decStack(bcg.mMaxStackSize);

	generateJMP_FALSE(mInstructionOffset + mInstructionCount + 2);
	//decStack();

	mBw.write(out);

	generateJMP(mInstructionOffset + startCount);
}

void ByteCodeGenerator::visit(::Beer::AST::ReturnNode * returnNode)
{
	NodeVisitor::visit(returnNode);

	ASSERT(returnNode->getArguments()->size() == mMethod->getReturnParameters()->size(), "Return node parameter count does not equal method return parameter count");

	if (returnNode->getArguments()->size())
	{
		generateSTORE(-static_cast<int16>(mMethod->getParameters()->size() + 1));
		//decStack();
	}

	generateRETURN(mBlock->getAllVariableCount() + mMethod->getParameters()->size() + 1, mBlock->getAllVariableCount());
	//decStack(mBlock->getVariableCount());
}

void ByteCodeGenerator::visit(::Beer::AST::ConstructorCallNode * call)
{
	SemanticAnalysis::Method * ctor = mBlock->getMethodCall(call);

	ASSERT(ctor, "Constructor is NULL");

	generateMOVE_TOP(1);
	//incStack();

	for (AST::ExpressionList::reverse_iterator it = call->getArguments()->rbegin(); it != call->getArguments()->rend(); it++)
	{
		(*it)->accept(this);
	}

	generateNEW(ctor->getDeclarator()->getId());
	//incStack();

	generateSPECIAL_INVOKE(ctor->getFullName()->getDescriptorId(), ctor->getParameterCount() + 1);
	//decStack(ctor->getParameters()->size() + 1);
}

void ByteCodeGenerator::visit(::Beer::AST::MethodCallNode * call)
{	
	SemanticAnalysis::Method * method = mBlock->getMethodCall(call);

	if (method)
	{
		if (method->getReturnParameters()->size() != 0)
		{
			generateMOVE_TOP(method->getReturnParameters()->size());
			//incStack(method->getReturnParameters()->size());
		}

		for (AST::ExpressionList::reverse_iterator it = call->getArguments()->rbegin(); it != call->getArguments()->rend(); it++)
		{
			(*it)->accept(this);
		}

		call->getInstance()->accept(this);

		SemanticAnalysis::Expression * inst = mBlock->getExpression(call->getInstance());

		if (method->getDeclarator() == inst->getType())
		{
			if (method->isOverriden())
			{
				generateINTERFACE_INVOKE(method->getFullName()->getDescriptorId(), method->getParameterCount() + 1);
			}
			else
			{
				generateINVOKE(method->getFullName()->getDescriptorId(), method->getParameterCount() + 1);
			}
		}
		else
		{
			generateINVOKE(method->getFullName()->getDescriptorId(), method->getParameterCount() + 1);
		}


		//decStack(method->getParameters()->size() + 1);

		if (call->getType() == AST::MethodCallNode::Statement)
		{
			if (method->getReturnParameters()->size() != 0)
			{
				generateMOVE_TOP(-static_cast<int16>(method->getReturnParameters()->size()));
				//decStack(method->getReturnParameters()->size());
			}
		}
	}
}

void ByteCodeGenerator::visit(::Beer::AST::IdentifierNode * identifier)
{
	SemanticAnalysis::Expression * info = mBlock->getExpression(identifier);

	ASSERT(info, "Info is NULL");

	switch (info->getKind())
	{
	case SemanticAnalysis::Expression::Member:
		generateTOP(0);
		//incStack();
		generateLOAD(info->getId());
		break;

	case SemanticAnalysis::Expression::Parameter:
	case SemanticAnalysis::Expression::Variable:
		generateTOP(info->getId());
		//incStack();
		break;

	default:
		throw std::exception("");
	}
}

void ByteCodeGenerator::visit(::Beer::AST::ThisNode * thisNode)
{
	generateTOP(0);
	//incStack();
}

void ByteCodeGenerator::visit(::Beer::AST::BooleanConstantNode * constant)
{
	generatePUSH_BOOL(constant->getValue());
	//incStack();
}

void ByteCodeGenerator::visit(::Beer::AST::CharacterConstantNode * constant)
{
	generatePUSH_CHAR(constant->getValue());
	//incStack();
}

void ByteCodeGenerator::visit(::Beer::AST::IntegerConstantNode * constant)
{
	generatePUSH_INT32(constant->getValue());
	//incStack();
}

void ByteCodeGenerator::visit(::Beer::AST::FloatConstantNode * constant)
{
	generatePUSH_FLOAT(constant->getValue());
	//incStack();
}

void ByteCodeGenerator::visit(::Beer::AST::StringConstantNode * constant)
{
	SemanticAnalysis::StringDescriptor * string = mProgram->getDescriptorDictionary()->getStringDescriptor(constant->getValue());

	ASSERT(string, "String descriptor is NULL");

	generatePUSH_STRING(string->getDescriptorId());
	//incStack();
}