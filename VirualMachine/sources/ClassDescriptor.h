#pragma once
#include "prereq.h"


namespace Beer
{
	class FileFormatConverter;
	class ClassFileDescriptor;
	class StringDescriptor;

	#pragma pack(push, 1)
	class ClassDescriptor
	{
	protected:
		//uint16 mLength; // deprecated
		uint8 mFlags;
		uint16 mNameId;
		uint16 mParentsLength;
		uint16 mParents; // should be used as array
		// uint16 mAttributesLength;
		// uint16 mAttributes; // should be used as array
		// uint16 mMethodsLength;
		// uint16 mMethods; // should be used as array

	public:
		// flags

		INLINE uint8 getFlags() const { return mFlags; }
		INLINE uint8& getFlags() { return mFlags; }

		// name

		INLINE uint16 getNameId() const { return mNameId; }
		INLINE uint16& getNameId() { return mNameId; }

		const StringDescriptor* getName(const ClassFileDescriptor* classFile) const;

		// parents

		INLINE uint16 getParentsLength() const { return mParentsLength; }
		INLINE uint16& getParentsLength() { return mParentsLength; }

		INLINE const uint16* getParents() const { return &mParents; }
		INLINE uint16* getParents() { return &mParents; }

		INLINE uint16 getParentId(uint16 i) const { return getParents()[i]; }
		INLINE uint16& getParentId(uint16 i) { return getParents()[i]; }

		// attributes

		INLINE uint16 getAttributesLength() const { return getParents()[getParentsLength()]; }
		INLINE uint16& getAttributesLength() { return getParents()[getParentsLength()]; }

		INLINE const uint16* getAttributes() const { return &getParents()[getParentsLength() + 1]; }
		INLINE uint16* getAttributes() { return &getParents()[getParentsLength() + 1]; }

		INLINE uint16 getAttributeId(uint16 i) const { return getAttributes()[i]; }
		INLINE uint16& getAttributeId(uint16 i) { return getAttributes()[i]; }

		// methods

		INLINE uint16 getMethodsLength() const { return getAttributes()[getAttributesLength()]; }
		INLINE uint16& getMethodsLength() { return getAttributes()[getAttributesLength()]; }

		INLINE const uint16* getMethodes() const { return &getAttributes()[getAttributesLength() + 1]; }
		INLINE uint16* getMethodes() { return &getAttributes()[getAttributesLength() + 1]; }

		INLINE uint16 getMethodId(uint16 i) const { return getMethodes()[i]; }
		INLINE uint16& getMethodId(uint16 i) { return getMethodes()[i]; }

		void convert(FileFormatConverter& format, ClassFileDescriptor* classFile);
	};
	#pragma pack(pop)
};
