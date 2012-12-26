#pragma once
#include "prereq.h"


namespace Beer
{
	class FileFormatConverter;
	class ClassFileDescriptor;

	#pragma pack(push, 1)
	class StringDescriptor
	{
	protected:
		uint8 mFlags;
		uint16 mLength;
		char mData; // should be used as pointer! will be wchar

	public:
		INLINE uint16 size() const { return mLength; }
		INLINE uint16& size() { return mLength; }

		INLINE const char* c_str() const
		{
			return &mData;
		}
		
		INLINE char* c_str()
		{
			return &mData;
		}

		void convert(FileFormatConverter& format, ClassFileDescriptor* classFile);
	};
	#pragma pack(pop)
};