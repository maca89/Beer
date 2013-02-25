#pragma once
#include "prereq.h"
#include "Character.h"
#include "ObjectClass.h"


namespace Beer
{
	class VirtualMachine;

	class CharacterClass : public Class
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			return Character::makeInlineValue('\0');
		}
	};

	class CharacterClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};