#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class EventsSectionNode : public SectionNode
		{
			virtual void accept(NodeVisitor * visitor);
		};
	}
}