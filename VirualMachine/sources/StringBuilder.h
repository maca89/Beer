#pragma once
#include "prereq.h"
#include "StringClass.h"
#include "VirtualMachine.h"


/*namespace Beer
{
	class StringBuilder
	{
	protected:
		VirtualMachine* mVM;
		Reference<String> mBuffer;
		String::LengthData mUsed;

	public:
		INLINE StringBuilder(VirtualMachine* vm) :  mVM(vm), mBuffer(), mUsed(0)
		{
		}

		INLINE void append(const char_t* data)
		{
			append(data, strlen(data));
		}

		INLINE void append(const char_t* data, int32 length)
		{
			if(mUsed + length > mBuffer->size())
			{
				String* newBuffer = mVM->createString(static_cast<String::LengthData>((mUsed + length) * 1.5f));
				StackRef<String> newBuffer(

				newBuffer->copyData(*mBuffer);
				mBuffer = StringClass::translate(newBuffer);
			}
			//mVM->createString(
		}
	};
};*/