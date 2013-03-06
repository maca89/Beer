#pragma once
#include "prereq.h"
#include "Object.h"
#include "Integer.h"
#include "ClassLoader.h"
#include "StackFrame.h"
#include "GenerationalGC.h"
#include "Character.h"


namespace Beer
{
	class VirtualMachine;
	class String;
	class Array;

	class StringPool
	{
	public:
		typedef std::map<const char16*, Reference<String>> StringMap;
		typedef StringMap::iterator iterator;

	protected:
		StringMap mStrings;

	public:
		//Reference<String> translate(VirtualMachine* vm, const char8* str);
		Reference<String> translate(Thread* thread, const char16* str);
		
		INLINE void clear()
		{
			mStrings.clear();
		}

		INLINE iterator begin()
		{
			return mStrings.begin();
		}

		INLINE iterator end()
		{
			return mStrings.end();
		}
	};

	class String : public Object
	{
	public:
		typedef Character::CharacterData CharacterData;
		typedef Integer::IntegerData LengthData;

	protected:
		// TODO: garbaged
		LengthData mLength;

		CharacterData mData; // should be used as array!

		static StringPool gPool; // TODO: get rid of
	
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

		
		static INLINE StringPool* gGetPool()
		{
			return &gPool;
		}

		static INLINE Reference<String> gTranslate(Thread* thread, const char16* str)
		{
			return gPool.translate(thread, str);
		}


		static void BEER_CALL createInstance(Thread* thread/*, StackFrame* frame*/, StackRef<Class> receiver, StackRef<String> ret);

		static void BEER_CALL init(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> ret);
		static void BEER_CALL getLength(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Integer> ret);
		static void BEER_CALL operatorGet(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Integer> index, StackRef<Character> ret);
		static void BEER_CALL operatorAddString(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<String> ret);
		static void BEER_CALL operatorAddInteger(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Integer> arg, StackRef<String> ret);
		static void BEER_CALL operatorAddFloat(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Float> arg, StackRef<String> ret);
		static void BEER_CALL operatorAddBoolean(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Boolean> arg, StackRef<String> ret);
		static void BEER_CALL operatorAddCharacter(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Character> arg, StackRef<String> ret);
		static void BEER_CALL operatorAddArray(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<Array> arg, StackRef<String> ret);
		
		static void BEER_CALL operatorEqual(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmaller(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorSmallerEqual(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreater(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorGreaterEqual(Thread* thread/*, StackFrame* frame*/, StackRef<String> receiver, StackRef<String> arg, StackRef<Boolean> ret);
	};

	class StringClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};