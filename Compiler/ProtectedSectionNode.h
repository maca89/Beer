#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class ProtectedSectionNode : public SectionNode
		{
			virtual void accept(NodeVisitor * visitor);
		};
	}
}