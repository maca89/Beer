#pragma once

#include "prereq.h"
#include "Node.h"

namespace Beer
{
	namespace AST
	{
		class BlockNode;

		class PropertyDefinitionNode : public Node
		{
		protected:

			std::string mType;
			std::string mName;
			BlockNode * mGetter;
			BlockNode * mSetter;

		public:

			INLINE PropertyDefinitionNode(const std::string & type, const std::string & name, BlockNode * getter, BlockNode * setter)
				:	mType(type),
					mName(name),
					mGetter(getter),
					mSetter(setter)
			{ }

			virtual ~PropertyDefinitionNode();


			INLINE const std::string & getType() const
			{
				return mType;
			}

			INLINE const std::string & getName() const
			{
				return mName;
			}

			INLINE BlockNode * getGetter()
			{
				return mGetter;
			}

			INLINE BlockNode * getSetter()
			{
				return mSetter;
			}


			virtual void accept(NodeVisitor * visitor);
		};
	}
}