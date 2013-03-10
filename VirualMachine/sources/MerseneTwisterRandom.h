#pragma once
#include "prereq.h"

namespace Beer
{
	class MerseneTwisterRandom
	{
	protected:
		std::mt19937_64 mGenerator;

	public:
		NOINLINE MerseneTwisterRandom()  { }
		NOINLINE MerseneTwisterRandom(int32 seed)  { mGenerator.seed(seed); }
		NOINLINE ~MerseneTwisterRandom() { }

		INLINE int64 generate() { return mGenerator(); }
	};
};