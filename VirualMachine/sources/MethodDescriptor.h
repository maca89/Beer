#pragma once
#include "prereq.h"

namespace Beer
{
	class FileFormatConverter;
	class ClassFileDescriptor;
	class StringDescriptor;

	#pragma pack(push, 1)
	class MethodDescriptor
	{
	protected:
		//uint16 mLength; // deprecated
		uint8 mFlags;
		uint16 mNameId;
		uint16 mParamsLength;
		uint16 mParams; // should be used as array
		// uint16 mReturnsLength;
		// uint16 mReturns;
		// uint16 mBytecodeId;

	public:
		// flags

		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }

		// name

		INLINE uint16 getNameId() const { return mNameId; }
		INLINE uint16& getNameId() { return mNameId; }

		const StringDescriptor* getName(ClassFileDescriptor* classFile) const;

		// params

		INLINE uint16 getParamsLength() const { return mParamsLength; }
		INLINE uint16& getParamsLength() { return mParamsLength; }

		INLINE const uint16* getParams() const { return &mParams; }
		INLINE uint16* getParams() { return &mParams; }

		INLINE uint16 getParamId(uint16 i) const { return getParams()[i]; }
		INLINE uint16& getParamId(uint16 i) { return getParams()[i]; }

		// returns

		INLINE uint16 getReturnsLength() const { return getParams()[getParamsLength()]; }
		INLINE uint16& getReturnsLength() { return getParams()[getParamsLength()]; }

		INLINE const uint16* getReturns() const { return &getParams()[getParamsLength() + 1]; }
		INLINE uint16* getReturns() { return &getParams()[getParamsLength() + 1]; }

		INLINE uint16 getReturnId(uint16 i) const { return getReturns()[i]; }
		INLINE uint16& getReturnId(uint16 i) { return getReturns()[i]; }

		// bytecode

		INLINE uint16 getBytecodeId() const { return getReturns()[getReturnsLength()]; }
		INLINE uint16& getBytecodeId() { return getReturns()[getReturnsLength()]; }

		// convert

		void convert(FileFormatConverter& format, ClassFileDescriptor* classFile);
	};
	#pragma pack(pop)
};