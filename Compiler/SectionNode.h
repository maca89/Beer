#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		class SectionNode : public Node { };

		typedef std::list<SectionNode *> Sections;
		typedef	std::list<SectionNode *>::iterator SectionsIterator;
	}
}
