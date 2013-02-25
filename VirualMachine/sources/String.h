#pragma once
#include "prereq.h"
#include "Object.h"
#include "Integer.h"
#include "Character.h"


namespace Beer
{
	class VirtualMachine;

	class String : public Object
	{
	public:
		typedef Character::CharacterData CharacterData;
		typedef Integer::IntegerData LengthData;

	protected:
		// TODO: garbaged
		LengthData mLength;

		CharacterData mData; // should be used as array!
	
	public:
		INLINE LengthData size() const
		{
			return mLength;
		}

		INLINE void size(LengthData value)
		{
			mLength = value;
		}

		INLINE const CharacterData* c_str() const
		{
			return &mData;
		}

		INLINE void copyData(const CharacterData* data)
		{
			//CopyMemory(&mData, data, mLength * sizeof(CharacterData));
			memcpy(&mData, data, mLength * sizeof(CharacterData));
			ensureTerminating();
		}

	#ifdef BEER_MULTIBYTE_STRINGS
		INLINE void copyData(const char16* data, LengthData length)
		{
			length = min(length, mLength);
			for(LengthData i = 0; i < length; i++)
			{
				(&mData)[i] = data[i];
			}
			ensureTerminating();
		}
	#endif

		INLINE void copyData(LengthData thisstart, LengthData length, const CharacterData* data)
		{
			memcpy(&(&mData)[thisstart], data, length * sizeof(CharacterData));
			ensureTerminating();
		}

		INLINE void copyData(String* string)
		{
			memcpy(&mData, string->c_str(), string->size() * sizeof(CharacterData));
			ensureTerminating();
		}

		INLINE int16 compare(String* string) const
		{
			return strcmp(c_str(), string->c_str());
		}

		INLINE void ensureTerminating()
		{
			(&mData)[mLength] = '\0'; // mData is always alocated with (mLength + 1) length
		}

		/*INLINE static std::wstring cast(const std::string& str)
		{
			return std::wstring(str.begin(), str.end());
		}

		INLINE static std::string cast(const std::wstring& str)
		{
			return std::string(str.begin(), str.end());
		}

		INLINE static std::wstring convert(const std::string& str)
		{
			// TODO
			return cast(str);
		}

		INLINE static std::string convert(const std::wstring& str)
		{
			// TODO
			return cast(str);
		}*/
	};
};