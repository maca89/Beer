#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class PropertyDefinitionNode : public Node
		{
		protected:

			std::string mType;
			std::string mName;
			BlockNode * mGetter;
			BlockNode * mSetter;

		public:

			INLINE PropertyDefinitionNode(std::string type, std::string name, BlockNode * getter, BlockNode * setter) : mType(type), mName(name), mGetter(getter), mSetter(setter) { }

			INLINE std::string getType() const { return mType; }
			INLINE std::string getName() const { return mName; }
			INLINE BlockNode * getGetter() { return mGetter; }
			INLINE BlockNode * getSetter() { return mSetter; }

			virtual void accept(NodeVisitor * visitor);
		};
	}
}