#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "Frame.h"


namespace Beer
{
	class VirtualMachine;
	class Class;
	class Integer;
	class Boolean;
	class String;

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

		static void BEER_CALL createInstance(Thread* thread/*, Frame* frame*/, StackRef<Class> receiver, StackRef<Character> ret);

		static void BEER_CALL init(Thread* thread/*, Frame* frame*/, StackRef<Character> receiver, StackRef<Character> ret1);

		static void BEER_CALL operatorString(Thread* thread/*, Frame* frame*/, StackRef<Character> receiver, StackRef<String> ret);
		static void BEER_CALL operatorInteger(Thread* thread/*, Frame* frame*/, StackRef<Character> receiver, StackRef<Integer> ret);
		static void BEER_CALL operatorEqual(Thread* thread/*, Frame* frame*/, StackRef<Character> receiver, StackRef<Character> arg, StackRef<Boolean> ret);
		static void BEER_CALL operatorNotEqual(Thread* thread/*, Frame* frame*/, StackRef<Character> receiver, StackRef<Character> arg, StackRef<Boolean> ret);
	};

	class CharacterClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};