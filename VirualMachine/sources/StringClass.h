#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "IntegerClass.h"
#include "CharacterClass.h"
#include "CopyGC.h"


namespace Beer
{
	class VirtualMachine;

	class String : public Object
	{
	public:
		typedef Character::CharacterData CharacterData;
		typedef Integer::IntegerData LengthData;

	protected:
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

	class StringPool
	{
	public:
		typedef std::map<const char16*, Reference<String>> StringMap;
		typedef StringMap::iterator iterator;

	protected:
		StringMap mStrings;

	public:
		//Reference<String> translate(VirtualMachine* vm, const char8* str);
		Reference<String> translate(VirtualMachine* vm, const char16* str);
		
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

	class StringClass : public ClassReflection
	{
	protected:
		StringPool mPool;

	public:
		INLINE StringPool* getPool()
		{
			return &mPool;
		}

		INLINE Reference<String> translate(VirtualMachine* vm, const char16* str)
		{
			return mPool.translate(vm, str);
		}

		// ClassReflection
		virtual String* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc);

		/*virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			String* str = this->ClassReflection::cloneShallow<String>(object, frame, gc);
			str->setData(object->getInstance<String>()->getData()); // TODO
			return str;
		}*/

		virtual void dump(Object* object, stringstream& out)
		{
			out << "\"" << object->getInstance<String>()->c_str() << "\"";
		};
	};

	class StringClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};