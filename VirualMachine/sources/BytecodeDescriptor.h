#pragma once
#include "prereq.h"


namespace Beer
{
	class FileFormatConverter;
	class ClassFileDescriptor;

	#pragma pack(push, 1)
	class BytecodeDescriptor
	{
	protected:
		uint8 mFlags;
		uint16 mInstrCount;
		uint16 mLength;
		byte mData; // should be used as pointer!

	public:
		// size

		INLINE uint16 getSize() const { return mLength; }
		INLINE uint16& getSize() { return mLength; }
		
		// flags

		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }

		// instr count

		INLINE uint16 getInstrCount() const { return mInstrCount; }
		INLINE uint16& getInstrCount() { return mInstrCount; }

		// data
		
		INLINE const byte* getData() const { return &mData; }
		INLINE byte* getData() { return &mData; }

		INLINE byte getByte(uint16 i) const { return getData()[i]; }
		INLINE byte& getByte(uint16 i) { return getData()[i]; }

		INLINE byte operator[] (uint16 i) const { return getByte(i); }
		INLINE byte& operator[] (uint16 i) { return getByte(i); }

		// convert

		void convert(FileFormatConverter& format, ClassFileDescriptor* classFile);
	};
	#pragma pack(pop)
};