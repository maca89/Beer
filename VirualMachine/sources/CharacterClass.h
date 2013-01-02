#pragma once
#include "prereq.h"
#include "ObjectClass.h"


namespace Beer
{
	class VirtualMachine;

	class Character : public Object
	{
	public:
		typedef char_t CharacterData;
		static const int SignatureBits = 4;

	protected:
		//CharacterData mData; // not used

	public:
		INLINE CharacterData getData() const
		{
			return (reinterpret_cast<uint32>(this) >> SignatureBits);
		}

		INLINE static Character* makeInlineValue(CharacterData data)
		{
			return reinterpret_cast<Character*>((data << SignatureBits) | 7);
		}
	};

	class CharacterClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			return Character::makeInlineValue('\0');
		}

		virtual Object* cloneShallow(Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			return object;
		}

		virtual void dump(Object* object, stringstream& out)
		{
			out << object->getInstance<Character>()->getData();
		};
	};

	class CharacterClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};