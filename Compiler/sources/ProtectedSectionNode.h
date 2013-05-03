#pragma once

#include "SectionNode.h"

namespace Beer
{
	namespace AST
	{
		class ProtectedSectionNode : public SectionNode
		{
			virtual void accept(NodeVisitor * visitor);
		};
	}
}