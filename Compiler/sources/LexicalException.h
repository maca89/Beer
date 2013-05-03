#pragma once

#include "prereq.h"

namespace Beer
{
	class LexicalException
	{
	protected:

		std::string mMessage;

	public:

		INLINE LexicalException(std::string message) : mMessage(message) { }

		INLINE const std::string & getMessage() const { return mMessage; }
	};
}