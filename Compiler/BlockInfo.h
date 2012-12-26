#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class MethodInfo;

		class BlockInfo : public DescriptorInfo 
		{
		protected:

			BlockInfo * mParent;
			AST::BlockNode * mBlock;

			std::map<AST::BlockNode *, BlockInfo *> mBlocks;

			bool mTerminated;

			parameter_id mParameterId;

			uint16 mStackSize;
			uint16 mMaxStackSize;

			ExpressionMap mExpressions;
			VariableMap mVariables;
						
		public:

			INLINE BlockInfo(AST::BlockNode * block, parameter_id parameterId)
				: mParent(NULL), mBlock(block), mTerminated(false), mParameterId(parameterId), mStackSize(0), mMaxStackSize(0) { }

			INLINE BlockInfo(BlockInfo * parent, AST::BlockNode * block, parameter_id parameterId)
				: mParent(parent), mTerminated(false), mBlock(block), mParameterId(parameterId), mStackSize(0), mMaxStackSize(0) { }

			INLINE BlockInfo * getParent() { return mParent; }

			INLINE AST::BlockNode * getBlock() { return mBlock; }

			INLINE bool isTerminated() const { return mTerminated; }

			INLINE parameter_id getParameterId() const { return mParameterId; }

			INLINE VariableMap * getVariables() { return &mVariables; }
			INLINE parameter_id getAllVariableCount() const { return mVariables.size() + (mParent ? mParent->getAllVariableCount() : 0); }
			INLINE parameter_id getVariableCount() const { return mVariables.size(); }

			uint16 getStackSize() const { return mStackSize; }

			BlockInfo * getBlock(AST::BlockNode * block);
			VariableInfo * getVariable(std::string name);
			ExpressionInfo * getExpression(AST::ExpressionNode * expression);

			INLINE void isTerminated(bool terminated) { mTerminated = terminated; }

			BlockInfo * addBlock(AST::BlockNode * block);

			/*INLINE void incrementStack(uint16 count = 1)
			{
				mStackSize += count;
				if (mStackSize > mMaxStackSize) mMaxStackSize = mStackSize;
			}

			INLINE void decrementStack(uint16 count = 1)
			{
				mStackSize -= count;
			}*/

			VariableInfo * declareVariable(ClassInfo * type, std::string name, Output & out);
			VariableInfo * defineVariable(ClassInfo * type, std::string name, Output & out);
			void assignVariable(std::string name);
			void addExpression(AST::ExpressionNode * expression, ExpressionInfo * expressionInfo);
			
		};
	}
}