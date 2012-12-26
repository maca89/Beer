#include "stdafx.h"

namespace BeerlangCompiler
{
	namespace SemanticAnalysis
	{
		class VariableInfo
		{
		protected:

			parameter_id mId;
			ClassInfo * mType;
			std::string mName;

			bool mDefined;

		public:

			INLINE VariableInfo(parameter_id id, ClassInfo * type, std::string name, bool defined = false) : mId(id), mType(type), mName(name), mDefined(defined) { }

			INLINE parameter_id getId() const { return mId; }

			INLINE ClassInfo * getType() { return mType; }
			INLINE std::string getName() { return mName; }
			INLINE bool isDefined() { return mDefined; }

			INLINE void isDefined(bool defined) { mDefined = defined; }
		};

		typedef std::map<std::string, VariableInfo *> VariableMap;
		typedef std::map<std::string, VariableInfo *>::iterator VariableMapIterator;
	}
}