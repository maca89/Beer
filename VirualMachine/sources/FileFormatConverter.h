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

		INLINE bool need()
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

		// user-check

		INLINE void convert(int16& n)
		{
			convert(reinterpret_cast<uint16&>(n));
		}

		INLINE void convert(uint16& n)
		{
			n = _byteswap_ushort(n);
		}

		INLINE void convert(int32& n)
		{
			convert(reinterpret_cast<uint32&>(n));
		}

		INLINE void convert(uint32& n)
		{
			n = _byteswap_ulong(n);
		}

		INLINE void convert(int64& n)
		{
			convert(reinterpret_cast<uint64&>(n));
		}

		INLINE void convert(uint64& n)
		{
			n = _byteswap_uint64(n);
		}
	};
};