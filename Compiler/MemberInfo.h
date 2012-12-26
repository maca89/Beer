#pragma once

#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		typedef uint16 member_id;
		typedef int8 member_flags;	

		class ClassInfo;

		class MemberInfo : public DescriptorInfo
		{
		public:

			enum MemberFlags
			{
				NoFlags = 0,

				Static = 4
			};


		protected:
			ClassInfo * mClass;

			member_id mId;
			member_flags mFlags;
			ClassInfo * mType;
			StringConstant * mName;
			std::string mFullName;

		public:

			INLINE MemberInfo(ClassInfo * cls, member_id id, member_flags flags, ClassInfo * type, StringConstant * name, std::string fullName)
				: mClass(cls), mId(id), mFlags(flags), mType(type), mName(name), mFullName(fullName) { }

			INLINE ClassInfo * getClass() { return mClass; }
			INLINE member_id getId() const { return mId; }
			INLINE member_flags getFlags() const { return mFlags; }
			INLINE ClassInfo * getType() { return mType; }
			INLINE StringConstant * getName() { return mName; }
			INLINE const std::string & getFullName() const { return mFullName; }
		};

		typedef std::list<MemberInfo *> MemberList;
		typedef std::list<MemberInfo *>::iterator MemberListIterator;

		typedef std::map<std::string, MemberInfo *> MemberMap;
		typedef std::map<std::string, MemberInfo *>::iterator MemberMapIterator;
	}
}