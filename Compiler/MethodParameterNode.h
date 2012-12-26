#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class MethodParameterNode : public Node
		{
		protected:

			std::string mType;
			std::string mName;
			ConstantNode * mValue;

		public:

			INLINE MethodParameterNode(std::string type, std::string name) : mType(type), mName(name), mValue(NULL) { }
			INLINE MethodParameterNode(std::string type, std::string name, ConstantNode * value) : mType(type), mName(name), mValue(value) { }
			virtual ~MethodParameterNode();

			INLINE std::string getType() const { return mType; }
			INLINE std::string getName() const { return mName; }
			INLINE ConstantNode * getValue() { return mValue; }

			virtual void accept(NodeVisitor * visitor);
		};

		typedef std::list<MethodParameterNode *> MethodParameterList;
		typedef std::list<MethodParameterNode *>::iterator MethodParameterListIterator;
	}
}

