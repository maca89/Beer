#pragma once
#include "prereq.h"


namespace Beer
{
	enum FileFormat
	{
		FORMAT_BIG_ENDIAN = 0x01,
		FORMAT_LITTLE_ENDIAN = 0x02
	};

	class FileFormatConverter
	{
	protected:
		FileFormat mSource;
		FileFormat mTarget;

	public:
		FileFormatConverter()
		{
			#if defined(WIN32)
				mTarget = FORMAT_LITTLE_ENDIAN;
				mSource = FORMAT_LITTLE_ENDIAN;
			#endif
		}

		INLINE bool needSwap()
		{
			return mTarget != mSource;
		}

		INLINE void setSource(FileFormat format)
		{
			mSource = format;
		}

		INLINE void setTarget(FileFormat format)
		{
			mTarget = format;
		}

		INLINE void convert(int8& n)
		{
			// nothing
		}

		INLINE void convert(uint8& n)
		{
			// nothing
		}

		INLINE void convert(int16& n)
		{
			convert(reinterpret_cast<uint16&>(n));
		}

		INLINE void convert(uint16& n)
		{
			if(needSwap())
			{
				doconvert(n);
			}
		}

		INLINE void convert(int32& n)
		{
			convert(reinterpret_cast<uint32&>(n));
		}

		INLINE void convert(uint32& n)
		{
			if(needSwap())
			{
				doconvert(n);
			}
		}

		INLINE void convert(int64& n)
		{
			convert(reinterpret_cast<uint64&>(n));
		}

		INLINE void convert(uint64& n)
		{
			if(needSwap())
			{
				doconvert(n);
			}
		}

	protected:
		INLINE void doconvert(uint16& n)
		{
			n = _byteswap_ushort(n);
		}

		INLINE void doconvert(uint32& n)
		{
			n = _byteswap_ulong(n);
		}

		INLINE void doconvert(uint64& n)
		{
			n = _byteswap_uint64(n);
		}
	};
};