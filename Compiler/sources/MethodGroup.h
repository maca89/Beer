#pragma once

#include "prereq.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class Class;
		class Method;

		typedef std::map<std::string, Method *> MethodMap;

		class MethodGroup 
		{
		protected:

			Class *			mDecl; // Class which declared method

			int				mAbstractMethods;

			MethodMap 		mMethods;			
									
		public:

			INLINE MethodGroup(Class * declarator)
				:	mDecl(declarator),
					mAbstractMethods(0)
			{ }

			INLINE Class * getDeclarator()
			{
				return mDecl;
			}
						
			INLINE bool isAbstract() const
			{
				return mAbstractMethods > 0;
			}
			
			INLINE MethodMap * getMethods()
			{
				return &mMethods;
			}
			
			INLINE bool hasMethod(const std::string & fullName)
			{
				return mMethods.find(fullName) != mMethods.end();
			}

			Method * getMethod(const std::string & qualifiedName);

			void addMethod(Method * method);

			void removeMethod(Method * method);


		protected:

			INLINE void incAbstractMethods()
			{
				mAbstractMethods++;
			}

			INLINE void decAbstractMethods()
			{
				mAbstractMethods--;
			}

		};
	}
}