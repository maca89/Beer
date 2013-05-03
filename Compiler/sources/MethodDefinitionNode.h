#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class BlockNode;
		class MethodParameterNode;		

		typedef std::list<MethodParameterNode *> MethodParameterList;
		typedef std::list<std::string> StringList;

		class MethodDefinitionNode : public Node
		{
		protected:

			StringList * mReturnParameters;
			std::string mName;
			MethodParameterList * mParameters;
			BlockNode * mBody;

		public:

			MethodDefinitionNode(const std::string & name, BlockNode * body = NULL);
			MethodDefinitionNode(const std::string & name, MethodParameterList * parameters, BlockNode * body = NULL);
			MethodDefinitionNode(const std::string & returnParameter, const std::string & name, BlockNode * body = NULL);
			MethodDefinitionNode(StringList * returnParameters, const std::string & name, BlockNode * body = NULL);
			MethodDefinitionNode(const std::string & returnParameter, const std::string & name, MethodParameterNode * parameter, BlockNode * body = NULL);

			INLINE MethodDefinitionNode(StringList * returnParameters, const std::string & name, MethodParameterList * parameters, BlockNode * body = NULL)
				:	mReturnParameters(returnParameters),
					mName(name),
					mParameters(parameters),
					mBody(body)
			{ }


			virtual ~MethodDefinitionNode();


			INLINE StringList * getReturnParameters()
			{
				return mReturnParameters;
			}

			INLINE const std::string & getName() const
			{
				return mName;
			}

			INLINE MethodParameterList * getParameters()
			{
				return mParameters;
			}

			INLINE BlockNode * getBody()
			{
				return mBody;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}

