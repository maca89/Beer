#pragma once
#include "prereq.h"
#include "FixedHeap.h"

namespace Beer
{
	class AlignedHeap : public FixedHeap
	{
	protected:

		uint8 mBitShift;

	public:

		AlignedHeap(uint8 bitSize);
		virtual ~AlignedHeap();

		virtual void init();
	};
}