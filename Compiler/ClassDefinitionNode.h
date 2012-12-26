#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class ClassDefinitionNode : public Node
		{
		protected:

			bool mNative;
			std::string mName;
			Sections * mSections;

		public:

			INLINE ClassDefinitionNode(bool native, std::string name, Sections * sections) : mNative(native), mName(name), mSections(sections) { }
			INLINE ClassDefinitionNode(std::string name, Sections * sections) : mNative(false), mName(name), mSections(sections) { }
			virtual ~ClassDefinitionNode();

			INLINE bool isNative() const { return mNative; }
			INLINE std::string getName() const { return mName; }
			INLINE Sections * getSections() { return mSections; }

			virtual void accept(NodeVisitor * visitor);
		};

		
		typedef std::list<ClassDefinitionNode *> ClassDefinitionList;
		typedef std::list<ClassDefinitionNode *>::iterator ClassDefinitionListIterator;
	}
}