#pragma once

#include "prereq.h"
#include "Member.h"
#include "Method.h"

namespace Beer
{
	class Output;

	namespace AST
	{
		class MethodParameterNode;

		typedef std::list<MethodParameterNode *> MethodParameterList;
		typedef std::list<std::string> StringList;
	}

	namespace SemanticAnalysis
	{
		class MethodGroup;
		class Program;

		typedef std::map<std::string, MethodGroup *> MethodGroupMap;
		typedef std::list<Method *> MethodList;
		typedef std::list<Member *> MemberList;
		typedef std::map<std::string, Member *> MemberMap;

		/*
		 * This class stores information about class.
		 */
		class Class : public Descriptor
		{
		public:

			typedef uint16 id;
			typedef int8 flags;

			enum Flag
			{
				NoFlag = 0,
				Native = 1,
				Abstract = 2
			};

		protected:
			Program *				mProgram;
			
			id						mId;
			flags					mFlags;

			StringDescriptor *		mName;

			std::list<Class *>		mParents; // List of parent classes

			Member::id				mMemberId; // ID which will be assigned to new member
			Method::id				mMethodId; // ID which will be assigned to new method

			MemberList				mMembers;
			MemberMap				mMemberMap;

			bool					mContainsConstructor;

			MethodList				mMethods;
			MethodGroupMap			mMethodGroups;

		public:

			Class(Program * program, StringDescriptor * name, flags flags);

			virtual ~Class();

			INLINE Program * getProgram()
			{
				return mProgram;
			}

			INLINE id getId() const
			{
				return mId;
			}

			INLINE StringDescriptor * getName() const
			{
				return mName;
			}

			INLINE std::list<Class *> *	getParents()
			{
				return &mParents;
			}
			
			INLINE flags getFlags() const
			{
				return mFlags;
			}

			INLINE MemberList * getMembers()
			{
				return &mMembers;
			}

			INLINE MethodList *	getMethods()
			{
				return &mMethods;
			}
			
			INLINE bool hasFlags(flags flags) const
			{
				return (mFlags & flags) == flags;
			}

			INLINE bool containsConstructor() const
			{
				return mContainsConstructor;
			}

			INLINE void setId(id id)
			{
				mId = id;
			}

			INLINE bool hasMember(const std::string & name) const
			{
				return mMemberMap.find(name) != mMemberMap.end();
			}

			bool hasMethod(const std::string & fullName) const;
			
			Member * getMember(const std::string & name);

			Method * getMethod(const std::string & qualifiedName);


			/*
			 * Returns true if this is same class as cls or cls is ancestor of this.
			 */
			bool isSubtype(Class * cls);
			

			/*
			 * Returns true if cls is ancestor of this.
			 */
			bool hasAncestor(Class * cls);

					
			void addParent(Class * parent, Output & out);			

			Member * addMember(Member::flags flags, const std::string & type, const std::string & name, Output & out);

			Method * addConstructor(Method::flags flags, AST::MethodParameterList * params, Output & out);
			Method * addConversion(Method::flags flags, const std::string & type, Output & out);
			Method * addMethod(Method::flags flags, const std::string & name, AST::MethodParameterList * params, Output & out);
			Method * addMethod(Method::flags flags, AST::StringList * returnParams, const std::string & name, AST::MethodParameterList * params, Output & out);
			void addMethod(Method * method);
			

		protected:

			void checkAbstract();

			MethodGroup * getMethodGroup(Class * declarator);
			
			void addMethodGroup(Class * declarator);

			Method * addMethod(Method::Type type, Method::flags flags, AST::StringList * returnParams, const std::string & name, AST::MethodParameterList * params, Output & out);
			
			void removeMethod(const std::string & qualifiedName);
			void removeMethod(Method * method);
		};
	}
}

