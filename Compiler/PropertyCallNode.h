#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class PropertyCallNode : public ExpressionNode
		{
		protected:

			ExpressionNode * mInstance;
			std::string mName;

		public:

			INLINE PropertyCallNode(ExpressionNode * instance, std::string name) : mInstance(instance), mName(name) { }
			virtual ~PropertyCallNode();

			INLINE ExpressionNode * getInstance() { return mInstance; }
			INLINE std::string getName() const { return mName; }

			virtual void accept(NodeVisitor * visitor);

		};
	}
}