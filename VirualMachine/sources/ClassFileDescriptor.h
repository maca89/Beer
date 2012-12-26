#pragma once
#include "prereq.h"


namespace Beer
{
	class FileFormatConverter;
	class StringDescriptor;

	#pragma pack(push, 1)
	class ClassFileDescriptor
	{
	public:
		#pragma pack(push, 1)
		struct Version
		{
			uint8 major;
			uint8 minor;

			INLINE uint16 getMajor()
			{
				return major;
			}

			INLINE uint16 getMinor()
			{
				return minor;
			}
		};
		#pragma pack(pop)

	public:
		uint8 mFileFormat; // LITTLE_ENDIAN = 0x01 | BIG_ENDIAN = 0x02
		uint8 mCompilerFlags; // DEBUG = 00000010 | RELEASE = 00000001
		uint16 mVersion; // první byte = major, druhý byte = minor
	
		uint16 mDescriptorsLength;
		uint16 mDescriptors; // should be used as pointer! will be uint32

		// uint16 mExternalClassesLength;
		// uint16 mDefinedClassesLength;
		// uint16 mClasses; // together with ExternalClasses

	public:
		// file format

		INLINE uint8 getFileFormat() const { return mFileFormat; }
		INLINE uint8& getFileFormat() { return mFileFormat; }

		// compiler flags

		INLINE uint8 getCompilerFlags() const { return mCompilerFlags; }
		INLINE uint8& getCompilerFlags() { return mCompilerFlags; }

		// version

		INLINE Version& getVersion() { return *reinterpret_cast<Version*>(&mVersion); }
		INLINE const Version& getVersion() const { return *reinterpret_cast<const Version*>(&mVersion); }
		
		// descriptors

		INLINE uint16 getDescriptorsLength() const { return mDescriptorsLength; }
		INLINE uint16& getDescriptorsLength() { return mDescriptorsLength; }

		INLINE uint16& getDescriptorAddr(uint16 i) { return getDescriptors()[i]; }
		INLINE uint16 getDescriptorAddr(uint16 i) const { return getDescriptors()[i]; }

		INLINE const void* getDescriptor(uint16 i) const { return (reinterpret_cast<const byte*>(&mFileFormat) + getDescriptorAddr(i)); }
		INLINE void* getDescriptor(uint16 i) { return (reinterpret_cast<byte*>(&mFileFormat) + getDescriptorAddr(i)); }

		template <typename T>
		INLINE const T* getDescriptor(uint16 i) const { return reinterpret_cast<const T*>(getDescriptor(i)); }

		template <typename T>
		INLINE T* getDescriptor(uint16 i) { return reinterpret_cast<T*>(getDescriptor(i)); }

		INLINE uint16* getDescriptors() { return &mDescriptors; }
		INLINE const uint16* getDescriptors() const { return &mDescriptors; }

		// external classes

		INLINE bool isExternalClass(uint16 i) const { return i < getExternalClassesLength(); }

		INLINE uint16& getExternalClassesLength() { return getDescriptors()[getDescriptorsLength()]; }
		INLINE uint16 getExternalClassesLength() const { return getDescriptors()[getDescriptorsLength()]; }

		// defined classes

		INLINE uint16& getDefinedClassesLength() { return getDescriptors()[getDescriptorsLength() + 1]; }
		INLINE uint16 getDefinedClassesLength() const { return getDescriptors()[getDescriptorsLength() + 1]; }

		// classes

		INLINE uint16* getClasses() { return &getDescriptors()[getDescriptorsLength() + 2]; }
		INLINE const uint16* getClasses() const { return &getDescriptors()[getDescriptorsLength() + 2]; }

		INLINE uint16& getClassId(uint16 i) { return getClasses()[i]; }
		INLINE uint16 getClassId(uint16 i) const { return getClasses()[i]; }

		const StringDescriptor* getClassName(uint16 i) const;

		// convert

		void convert(FileFormatConverter& format);
	};
	#pragma pack(pop)
};