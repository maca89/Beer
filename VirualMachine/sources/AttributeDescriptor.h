#pragma once
#include "prereq.h"


namespace Beer
{
	class FileFormatConverter;
	class ClassFileDescriptor;
	class StringDescriptor;

	#pragma pack(push, 1)
	class AttributeDescriptor
	{
	protected:
		//uint16 mLength; // deprecated
		uint8 mFlags;
		uint16 mNameId;
		uint16 mTypeId;

	public:
		// flags

		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }
		
		// name

		INLINE uint16 getNameId() const { return mNameId; }
		INLINE uint16& getNameId() { return mNameId; }

		const StringDescriptor* getName(const ClassFileDescriptor* classFile) const;

		// type

		INLINE uint16 getTypeId() const { return mTypeId; }
		INLINE uint16& getTypeId() { return mTypeId; }

		// convert

		void convert(FileFormatConverter& format, ClassFileDescriptor* classFile);
	};
	#pragma pack(pop)
};