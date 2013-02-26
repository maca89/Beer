#pragma once
#include "prereq.h"
#include "Object.h"


namespace Beer
{
	class VirtualMachine;

	class Boolean : public Object
	{
	public:
		typedef bool BooleanData;
		static const int SignatureBits = 4;

	protected:
		//BooleanData mData; // not used

	public:
		INLINE BooleanData getData() const
		{
			if(this == NULL)
			{
				return false; // just a workaround, TODO
			}

			return (reinterpret_cast<uint32>(this) >> SignatureBits) != 0;
		}

		NOINLINE void toString(string& out)
		{
			stringstream ss;
			ss << getData();
			ss >> out;
		}

		INLINE static Boolean* makeInlineValue(BooleanData data)
		{
			return reinterpret_cast<Boolean*>((data << SignatureBits) | 3);
		}
	};
};