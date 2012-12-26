#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		typedef int16 method_id;
		typedef int8 method_flags;
		
		class ClassInfo;

		class MethodInfo : public DescriptorInfo
		{
		public:

			enum MethodType
			{
				Constructor,
				Conversion,
				Method
			};

			enum MethodFlag
			{
				NoFlag = 0,
				Native = 1
			};

		protected:

			ClassInfo * mClass;

			method_id mId;
			method_flags mFlags;
			StringConstant * mName;
			StringConstant * mFullName;
			MethodType mType;
			MethodParameterList mReturnParameters;
			MethodParameterMap mReturnParameterMap;
			MethodParameterList mParameters;
			MethodParameterMap mParameterMap;
			bool mTerminated;

			std::map<AST::ExpressionNode *, MethodInfo *> mMethodCalls;

			BlockInfo * mBlock;

			parameter_id mParameterId;

			bool mUsed;

		public:

			INLINE MethodInfo(ClassInfo * cls, method_id id, method_flags flags, StringConstant * name, StringConstant * fullName, MethodType type)
				: mClass(cls), mId(id), mFlags(flags), mName(name), mFullName(fullName), mType(type), mTerminated(false), mParameterId(FIRST_PARAMETER_ID) { }
			
			INLINE ClassInfo * getClass() { return mClass; }
			INLINE method_id getId() const { return mId; }
			INLINE method_flags getFlags() const { return mFlags; }
			INLINE StringConstant * getName() { return mName; }
			INLINE StringConstant * getFullName() { return mFullName; }
			INLINE MethodType getType() { return mType; }

			INLINE bool hasFlags(method_flags flags) const { return (mFlags & flags) == flags; }

			parameter_id getParameterCount() const;

			MethodParameterInfo * getParameter(std::string name);

			INLINE MethodParameterList * getParameters() { return &mParameters; }
			INLINE MethodParameterList * getReturnParameters() { return &mReturnParameters; }

			INLINE bool isTerminated() const { return mTerminated; }

			INLINE BlockInfo * getBlock() { return mBlock; }

			MethodInfo * getMethodCall(AST::ExpressionNode * call);

			INLINE void setName(StringConstant * name) { mName = name; }
			INLINE void setFullName(StringConstant * fullName) { mFullName = fullName; }

			INLINE void isTerminated(bool terminated) { mTerminated = terminated; }

			BlockInfo * setBlock(AST::BlockNode * block);

			void addMethodCall(AST::ExpressionNode * call, MethodInfo * info);

			

			MethodParameterInfo * addParameter(std::string type, std::string name, Output & out);
			MethodParameterInfo * addReturnParameter(std::string type, Output & out);

		};

		typedef std::list<MethodInfo *> MethodList;
		typedef std::list<MethodInfo *>::iterator MethodListIterator;

		typedef std::map<std::string, MethodInfo *> MethodMap;
		typedef std::map<std::string, MethodInfo *>::iterator MethodMapIterator;

		typedef std::map<int, MethodInfo *> OperatorMap;
		typedef std::map<int, MethodInfo *>::iterator OperatorMapIterator;
	}
}

