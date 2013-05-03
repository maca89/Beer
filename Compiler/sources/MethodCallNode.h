#pragma once

#include "prereq.h"
#include "ExpressionNode.h"
#include "StatementNode.h"

namespace Beer
{
	namespace AST
	{
		typedef std::list<ExpressionNode *> ExpressionList;

		class MethodCallNode : public ExpressionNode, public StatementNode
		{
		public:
			enum Type
			{
				Expression,
				Statement
			};

		protected:

			ExpressionNode * mInstance;
			std::string mName;
			ExpressionList * mArguments;
			Type mType;

		public:

			INLINE MethodCallNode(const std::string & name, ExpressionList * arguments)
				:	mInstance(NULL),
					mName(name),
					mArguments(arguments),
					mType(Expression)
			{ }

			MethodCallNode(ExpressionNode * instance, const std::string & name);
			MethodCallNode(ExpressionNode * instance, const std::string & name, ExpressionNode * argument);

			INLINE MethodCallNode(ExpressionNode * instance, std::string name, ExpressionList * arguments)
				:	mInstance(instance),
					mName(name),
					mArguments(arguments)
			{ }


			virtual ~MethodCallNode();


			INLINE ExpressionNode * getInstance() const
			{
				return mInstance;
			}

			INLINE const std::string & getName()
			{
				return mName;
			}

			INLINE ExpressionList * getArguments()
			{
				return mArguments;
			}

			INLINE Type getType() const
			{
				return mType;
			}


			INLINE void setInstance(AST::ExpressionNode * instance)
			{
				mInstance = instance;
			}

			INLINE void setType(Type type)
			{
				mType = type;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}