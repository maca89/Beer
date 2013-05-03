#pragma once

namespace Beer
{
	namespace AST
	{		
		class NodeVisitor;

		class Node
		{
		public:

			virtual ~Node()
			{ }

			virtual void accept(NodeVisitor * visitor) = 0;
		};
	}
}