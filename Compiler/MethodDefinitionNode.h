#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class MethodDefinitionNode : public Node
		{
		protected:

			StringList * mReturnParameters;
			std::string mName;
			MethodParameterList * mParameters;
			BlockNode * mBody;

		public:

			MethodDefinitionNode(std::string name, BlockNode * body = NULL);
			MethodDefinitionNode(std::string name, MethodParameterList * parameters, BlockNode * body = NULL);
			MethodDefinitionNode(std::string returnParameter, std::string name, BlockNode * body = NULL);
			MethodDefinitionNode(StringList * returnParameters, std::string name, BlockNode * body = NULL);
			MethodDefinitionNode(std::string returnParameter, std::string name, MethodParameterNode * parameter, BlockNode * body = NULL);
			INLINE MethodDefinitionNode(StringList * returnParameters, std::string name, MethodParameterList * parameters, BlockNode * body = NULL)
				: mReturnParameters(returnParameters), mName(name), mParameters(parameters), mBody(body) { }

			virtual ~MethodDefinitionNode();

			INLINE StringList * getReturnParameters() { return mReturnParameters; }
			INLINE std::string getName() const { return mName; }
			INLINE MethodParameterList * getParameters() { return mParameters; }
			INLINE BlockNode * getBody() { return mBody; }

			virtual void accept(NodeVisitor * visitor);
		};

		typedef std::list<MethodDefinitionNode *> MethodDefinitionList;
		typedef std::list<MethodDefinitionNode *>::iterator MethodDefinitionListIterator;
	}
}

