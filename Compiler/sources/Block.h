#pragma once

#include "prereq.h"
#include "Descriptor.h"
#include "Variable.h"

namespace Beer
{
	class Output;

	namespace AST
	{
		class BlockNode;
		class ExpressionNode;
	}

	namespace SemanticAnalysis
	{
		class Expression;
		class Method;
		class Variable;

		typedef std::map<AST::ExpressionNode *, Expression *> ExpressionMap;
		typedef std::map<std::string, Variable *> VariableMap;

		/*
		 * This class stores information (termination, expression results, variables) about
		 * block of code.
		 */
		class Block : public Descriptor 
		{
		protected:

			Block *										mParent;
			AST::BlockNode *							mBlock;

			std::map<AST::BlockNode *, Block *>			mBlocks; // Map of child blocks

			bool										mTerminated;

			MethodParameter::id							mParameterId; // ID which will be assigned to new variable

			std::map<AST::ExpressionNode *, Method *>	mMethodCalls;
			ExpressionMap								mExpressions;
			VariableMap									mVariables;
						
		public:

			INLINE	Block(AST::BlockNode * block, MethodParameter::id parameterId)
				:	mParent(NULL),
					mBlock(block),
					mTerminated(false),
					mParameterId(parameterId)
			{ }


			INLINE	Block(Block * parent, AST::BlockNode * block, MethodParameter::id parameterId)
				:	mParent(parent),
					mTerminated(false),
					mBlock(block),
					mParameterId(parameterId)
			{ }

			/*
			 * Returns parent block info if any exists.
			 */
			INLINE Block * getParent()
			{
				return mParent;
			}

			/*
			 * Return blocks of code.
			 */
			INLINE AST::BlockNode * getBlock()
			{
				return mBlock;
			}


			INLINE bool isTerminated() const
			{
				return mTerminated;
			}


			INLINE MethodParameter::id getParameterId() const
			{
				return mParameterId;
			}

			/*
			 * Returns all variables declared in this block info.
			 */
			INLINE VariableMap * getVariables()
			{
				return &mVariables;
			}

			/*
			 * Returns count of all variables, even those from parent block info.
			 */
			INLINE MethodParameter::id getAllVariableCount() const
			{
				return mVariables.size() + (mParent ? mParent->getAllVariableCount() : 0);
			}

			/*
			 * Returns count of variable only from this block info (not from its parents).
			 */
			INLINE MethodParameter::id getVariableCount() const
			{
				return mVariables.size();
			}


			Method * getMethodCall(AST::ExpressionNode * call);

			/*
			 * Returns block info for given child block of code.
			 */
			Block * getBlock(AST::BlockNode * block);

			Variable * getVariable(std::string name);

			/*
			 * Returns expression infor for given expression.
			 */
			Expression * getExpression(AST::ExpressionNode * expression);
			

			INLINE void isTerminated(bool terminated)
			{
				mTerminated = terminated;
			}


			void addMethodCall(AST::ExpressionNode * call, Method * method);

			/*
			 * Adds new child block to this block info. Also creates new block for given block
			 * and returns it.
			 */
			Block *	addBlock(AST::BlockNode * blockNode);

			/*
			 * Declares variable, creates variable info and returns it.
			 */
			Variable * declareVariable(Class * type, const std::string & name, Output & out);

			/*
			 * Defines variable, creates variable info and returns it.
			 */
			Variable * defineVariable(Class * type, const std::string & name, Output & out);


			/*
			 * Sets variable as defined.
			 */
			void assignVariable(const std::string & name);

			void addExpression(AST::ExpressionNode * expression, Expression * expr);
		};
	}
}