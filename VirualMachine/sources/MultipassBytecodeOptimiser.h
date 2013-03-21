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
		virtual Bytecode* optimise(Thread* thread, StackRef<Method> method, Bytecode* bc);
	};
};