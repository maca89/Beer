#pragma once
#include "prereq.h"


namespace Beer
{
	class FileFormatConverter;
	class ClassFileDescriptor;

	#pragma pack(push, 1)
	class StringDescriptor
	{	
	public:
		enum
		{
			FLAG_FILE_ENCODED = 0x00,
			FLAG_MACHINE_ENCODED = 0x01
		};

	protected:
		uint8 mFlags;
		uint16 mSize; // size in bytes!
		char16 mData; // should be used as pointer!

	public:
		//INLINE uint16 size() const { return mSize; }

		// size - in chars!

		INLINE uint16 getSize() const { return mSize / sizeof(char16); }
		
		// flags

		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }
		
		INLINE void markAsMachineEncoded() { markFlag(FLAG_MACHINE_ENCODED); }
		INLINE bool isMachineEncoded() const { return hasFlag(FLAG_MACHINE_ENCODED); }

		INLINE bool hasFlag(uint8 n) const { return (mFlags & n) == n; }
		INLINE void markFlag(uint8 n) { mFlags |= n; }

		// data

		INLINE const char16* c_str() const
		{
			return &mData;
		}
		
		/*INLINE char* c_str()
		{
			return &mData;
		}*/

		void convert(FileFormatConverter& format, ClassFileDescriptor* classFile);
	};
	#pragma pack(pop)
};