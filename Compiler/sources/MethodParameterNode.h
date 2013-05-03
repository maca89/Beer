#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class ConstantNode;

		class MethodParameterNode : public Node
		{
		protected:

			std::string mType;
			std::string mName;
			ConstantNode * mValue;

		public:

			INLINE MethodParameterNode(const std::string & type, std::string name)
				:	mType(type),
					mName(name),
					mValue(NULL)
			{ }
			
			INLINE MethodParameterNode(const std::string & type, std::string name, ConstantNode * value)
				:	mType(type),
					mName(name),
					mValue(value)
			{ }


			virtual ~MethodParameterNode();


			INLINE const std::string & getType() const
			{
				return mType;
			}

			INLINE const std::string & getName() const
			{
				return mName;
			}

			INLINE ConstantNode * getValue()
			{
				return mValue;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}

