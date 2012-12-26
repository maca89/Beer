#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class ExpressionInfo 
		{
		public:

			enum Kind
			{
				Variable,
				Parameter,
				Member,
				Expression
			};


		protected:
			uint16 mId;
			Kind mKind;
			ClassInfo * mType;

		public:

			INLINE ExpressionInfo(ClassInfo * type)
				: mId(0), mKind(Expression), mType(type) { }

			INLINE ExpressionInfo(uint16 id, Kind kind, ClassInfo * type)
				: mId(id), mKind(kind), mType(type) { }

			INLINE uint16 getId() const { return mId; }
			INLINE Kind getKind() const { return mKind; }
			INLINE ClassInfo * getType() { return mType; }
		};

		typedef std::map<AST::ExpressionNode *, ExpressionInfo *> ExpressionMap;
		typedef std::map<AST::ExpressionNode *, ExpressionInfo *>::iterator ExpressionMapIterator;
	}
}