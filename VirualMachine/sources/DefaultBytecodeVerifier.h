#pragma once
#include "prereq.h"
#include "BytecodePass.h"


namespace Beer
{
	class DefaultBytecodeVerifier : public BytecodeVerifier
	{
	protected:

	public:
		INLINE DefaultBytecodeVerifier() {}
		virtual ~DefaultBytecodeVerifier() {}

		// BytecodeVerifier
		virtual void verify(Thread* thread, Method* method, const TemporaryBytecode& bc);
	};
};