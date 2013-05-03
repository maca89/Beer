#pragma once

#include "prereq.h"
#include "SectionNode.h"

namespace Beer
{
	namespace AST
	{
		class EventsSectionNode : public SectionNode
		{
			virtual void accept(NodeVisitor * visitor);
		};
	}
}