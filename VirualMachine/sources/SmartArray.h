#pragma once
#include "prereq.h"


namespace Beer
{
	template <typename T>
	class SmartArray
	{
	protected:
		T* mData;
		size_t mLength;

	public:
		INLINE SmartArray() : mData(NULL), mLength(0)
		{
		}

		INLINE SmartArray(size_t length) : mData(NULL), mLength(length)
		{
			mData = new T[mLength];
		}

		SmartArray(const SmartArray& arr) : mData(NULL), mLength(arr.mLength)
		{
			mData = new T[mLength];
			for(size_t i = 0; i < mLength; i++)
			{
				mData[i] = arr.mData[i];
			}
		}

		INLINE ~SmartArray()
		{
			delete[] mData;
		}

		SmartArray& operator= (const SmartArray& arr)
		{
			SMART_DELETE_ARR(mData);
			mLength = arr.mLength;

			mData = new T[mLength];
			for(size_t i = 0; i < mLength; i++)
			{
				mData[i] = arr.mData[i];
			}

			return *this;
		}

		INLINE T& operator[] (size_t index)
		{
			return mData[index];
		}

		INLINE const T& operator[] (size_t index) const
		{
			return mData[index];
		}

		INLINE size_t size() const
		{
			return mLength;
		}
	};
};