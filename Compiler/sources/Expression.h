#pragma once

#include "prereq.h"

namespace Beer
{
	namespace AST
	{
		class ExpressionNode;
	}

	namespace SemanticAnalysis
	{
		class Class;

		/*
		 * This class stores information about expressions. If expression kind is
		 * variable, parameter or member then id contains id of those items.
		 */
		class Expression 
		{
		public:

			enum Kind
			{
				Variable,
				Parameter,
				Member,
				Expr
			};


		protected:

			int16			mId;
			Kind			mKind;
			Class *			mType; // Type of result

		public:

			INLINE Expression(Class * type)
				:	mId(0),
					mKind(Expr),
					mType(type)
			{ }


			INLINE Expression(int16 id, Kind kind, Class * type)
				:	mId(id),
					mKind(kind),
					mType(type)
			{ }



			INLINE int16 getId() const
			{
				return mId;
			}


			INLINE Kind getKind() const
			{
				return mKind;
			}


			INLINE Class * getType()
			{
				return mType;
			}
		};

		typedef std::map<AST::ExpressionNode *, Expression *> ExpressionMap;
		typedef std::map<AST::ExpressionNode *, Expression *>::iterator ExpressionMapIterator;
	}
}