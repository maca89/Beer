#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		typedef uint16 class_id;
		typedef int8 class_flags;

		class ProgramInfo;

		class ClassInfo : public DescriptorInfo
		{
		public:

			enum ClassFlag
			{
				NoFlag = 0,
				Native = 1,


				Interface = 8
			};

		protected:
			ProgramInfo * mProgram;
			
			class_id mId;
			class_flags mFlags;

			StringConstant * mName;

			member_id mMemberId;
			method_id mMethodId;

			MemberList mMembers;
			MemberMap mMemberMap;		

			bool mContainsConstructor;

			MethodList mMethods;
			MethodMap mMethodMap;

			bool mUsed;

		public:

			ClassInfo(ProgramInfo * program, StringConstant * name, class_flags flags);

			INLINE ProgramInfo * getProgram() { return mProgram; }
			INLINE class_id getId() const { return mId; }
			INLINE StringConstant * getName() const { return mName; }
			INLINE class_flags getFlags() const { return mFlags; }

			INLINE MemberList * getMembers() { return &mMembers; }
			INLINE MethodList * getMethods() { return &mMethods; }

			INLINE bool hasFlags(class_flags flags) const { return (mFlags & flags) == flags; }

			INLINE bool containsConstructor() { return mContainsConstructor; }

			INLINE bool isUsed() const { return mUsed; }

			INLINE void setId(class_id id) { mId = id; }

			MemberInfo * getMember(std::string name);
			MethodInfo * getMethod(std::string fullName);

			INLINE void isUsed(bool used) { mUsed = used; }

			MemberInfo * addMember(member_flags flags, std::string type, std::string name, Output & out);
			MethodInfo * addConstructor(AST::MethodParameterList * parameters, Output & out);
			MethodInfo * addConversion(std::string type, Output & out);
			MethodInfo * addMethod(std::string name, AST::MethodParameterList * parameters, Output & out);
			MethodInfo * addMethod(AST::StringList * returnParameters, std::string name, AST::MethodParameterList * parameters, Output & out);
			//MethodInfo * addOperator(std::string type, int op, AST::MethodParameters * parameters, Output * out);

			std::string createMethodFullName(std::string methodName, AST::MethodParameterList * parameters);
			std::string createMethodFullName(std::string methodName, AST::ExpressionList * arguments, BlockInfo * block);
			/*std::string createMethodFullName(int op);
			std::string createMethodFullName(int op, AST::MethodParameterNode * rightOperand);
			std::string createMethodFullName(int op, AST::ExpressionNode * rightOperand, BlockInfo * block);

			std::string createMethodFullName(int op, AST::MethodParameters * operands);*/

		};

		typedef std::list<ClassInfo *> ClassList;
		typedef std::list<ClassInfo *>::iterator ClassListIterator;

		typedef std::map<std::string, ClassInfo *> ClassMap;
		typedef std::map<std::string, ClassInfo *>::iterator ClassMapIterator;
	}
}

