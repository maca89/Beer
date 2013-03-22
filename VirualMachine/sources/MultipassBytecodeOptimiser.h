#pragma once
#include "prereq.h"
#include "BytecodePass.h"

namespace Beer
{
	class MultipassBytecodeOptimiser : public BytecodeOptimiser
	{
	public:

	protected:

	public:
		MultipassBytecodeOptimiser();
		~MultipassBytecodeOptimiser();

		// BytecodeOptimiser
		virtual void optimise(Thread* thread, StackRef<Method> method, const TemporaryBytecode& bc, TemporaryBytecode& out_bc);
	};
};