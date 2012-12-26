#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "IntegerClass.h"


namespace Beer
{
	class VirtualMachine;

	class String : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();
	public:
		typedef int8 CharacterData;
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

		INLINE const char* c_str() const
		{
			return &mData;
		}

		INLINE void copyData(const char* data)
		{
			memcpy(&mData, data, mLength * sizeof(CharacterData));
			ensureTerminating();
		}

		INLINE void copyData(LengthData thisstart, LengthData length, const char* data)
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
	};

	class StringClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual String* createInstance(StackFrame* frame, GarbageCollector* gc);

		/*virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			String* str = this->ClassReflection::cloneShallow<String>(object, frame, gc);
			str->setData(object->getInstance<String>()->getData()); // TODO
			return str;
		}*/

		virtual void dump(Object* object, std::stringstream& out)
		{
			out << "\"" << object->getInstance<String>()->c_str() << "\"";
		};
	};

	class StringClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};