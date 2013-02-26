#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class VirtualMachine;

	class Float : public Object
	{
	public:
		typedef float64 FloatData;

	protected:
		FloatData mData;

	public:
		INLINE FloatData getData() const
		{
			if(this == NULL)
			{
				return 0.0f; // just a workaround, TODO
			}

			return mData;
		}

		INLINE void setData(FloatData data)
		{
			mData = data;
		}
	};
};