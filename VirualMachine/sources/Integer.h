#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class Integer : public Object
	{
	public:
		typedef int64 IntegerData;
		static const int SignatureBits = 2;

	protected:
		IntegerData mData;

	public:
		INLINE IntegerData getData() const
		{
			if(isInlineValue(this))
			{
				return getInlineValue(this);
			}
			return mData;
		}

		INLINE void setNonInlineValue(IntegerData data)
		{
			mData = data;
		}

		NOINLINE void toString(string& out)
		{
			stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static bool canBeInlineValue(IntegerData data)
		{
			// TODO: negative numbers
			return (data >> (sizeof(IntegerData) * 8 - SignatureBits)) == 0;
		}

		INLINE static Integer* makeInlineValue(IntegerData data)
		{
			return reinterpret_cast<Integer*>((data << SignatureBits) | 1);
		}

		INLINE static IntegerData getInlineValue(const Integer* data)
		{
			return reinterpret_cast<IntegerData>(data) >> SignatureBits;
		}

		/*INLINE bool operator < (const Integer& int2) { return getData() < int2.getData(); }
		INLINE bool operator <= (const Integer& int2) { return getData() <= int2.getData(); }
		INLINE bool operator > (const Integer& int2) { return getData() > int2.getData(); }
		INLINE bool operator >= (const Integer& int2) { return getData() >= int2.getData(); }
		INLINE bool operator == (const Integer& int2) { return getData() == int2.getData(); }
		INLINE bool operator != (const Integer& int2) { return getData() != int2.getData(); }
		INLINE IntegerData operator + (const Integer& int2) { return getData() + int2.getData(); }
		INLINE IntegerData operator - (const Integer& int2) { return getData() - int2.getData(); }
		INLINE IntegerData operator * (const Integer& int2) { return getData() * int2.getData(); }*/
	};
};