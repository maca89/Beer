#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace AST
	{
		
		typedef std::list<std::string> StringList;
		typedef std::list<std::string>::iterator StringListIterator;

		class NodeVisitor;

		class Node
		{
		public:

			virtual ~Node() { }

			virtual void accept(NodeVisitor * visitor) = 0;
		};
	}
}