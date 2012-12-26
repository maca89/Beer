#include "stdafx.h"

namespace BeerlangCompiler
{
	class SemanticException
	{
	protected:

		std::string mMessage;

	public:

		INLINE SemanticException(std::string message) : mMessage(message) { }

		INLINE const std::string & getMessage() const { return mMessage; }
	};
}