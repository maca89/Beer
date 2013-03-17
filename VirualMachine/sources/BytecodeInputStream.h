#pragma once
#include "prereq.h"


namespace Beer
{
	class BytecodeInputStream
	{
	protected:
		byte* mData;
		uint32 mDataLength;
		uint32 mCounter;

	public:
		BytecodeInputStream(byte* data, uint32 dataLength)
			: mData(data), mDataLength(dataLength), mCounter(0)
		{
		}

		template <typename T>
		T read()
		{
			return read<T>(sizeof(T));
		}

		template <typename T>
		T read(uint32 length)
		{
			T t = *reinterpret_cast<T*>(&mData[mCounter]);
			move(length);
			return t;
		}

		template <typename T>
		T* readPtr()
		{
			return readPtr<T>(sizeof(T*));
		}

		template <typename T>
		T* readPtr(uint32 length)
		{
			T* t = reinterpret_cast<T*>(&mData[mCounter]);
			move(length);
			return t;
		}

		void move(uint32 length)
		{
			mCounter += length;
		}

		bool end() const
		{
			return mCounter >= mDataLength;
		}
	};
};