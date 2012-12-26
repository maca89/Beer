#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		typedef int16 parameter_id;
		typedef int8 parameter_flags;

		class ClassInfo;

		class MethodParameterInfo : public DescriptorInfo
		{
		public:

			enum ParameterFlag
			{
				NoFlag
			};

		protected:

			parameter_id mId;
			parameter_flags mFlags;
			ClassInfo * mType;
			StringConstant * mName;
			
		public:

			INLINE MethodParameterInfo(parameter_id id, parameter_flags flags, ClassInfo * type, StringConstant * name)
				: mId(id), mFlags(flags), mType(type), mName(name) { }

			INLINE parameter_id getId() const { return mId; }
			INLINE parameter_flags getFlags() const { return mFlags; }
			INLINE ClassInfo * getType() { return mType; }
			INLINE StringConstant * getName() const { return mName; }

		};

		typedef std::list<MethodParameterInfo *> MethodParameterList;
		typedef std::list<MethodParameterInfo *>::iterator MethodParameterListIterator;

		typedef std::map<std::string, MethodParameterInfo *> MethodParameterMap;
		typedef std::map<std::string, MethodParameterInfo *>::iterator MethodParameterMapIterator;


	}
}