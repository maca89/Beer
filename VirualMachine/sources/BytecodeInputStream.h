#pragma once
#include "prereq.h"


namespace Beer
{
	class BytecodeInputStream
	{
	protected:
		const byte* mData;
		uint32 mDataLength;
		uint32 mCounter;

	public:
		BytecodeInputStream(const void* data, uint32 dataLength)
			: mData(reinterpret_cast<const byte*>(data)), mDataLength(dataLength), mCounter(0)
		{
		}

		template <typename T>
		const T read()
		{
			return read<T>(sizeof(T));
		}

		template <typename T>
		const T read(uint32 length)
		{
			const T t = *reinterpret_cast<const T*>(&mData[mCounter]);
			move(length);
			return t;
		}

		template <typename T>
		const T* readPtr()
		{
			return readPtr<T>(sizeof(T*));
		}

		template <typename T>
		const T* readPtr(uint32 length)
		{
			const T* t = reinterpret_cast<const T*>(&mData[mCounter]);
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