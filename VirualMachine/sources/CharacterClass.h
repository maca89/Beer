#pragma once
#include "prereq.h"
#include "Character.h"
#include "Class.h"
#include "ClassLoader.h"


namespace Beer
{
	class VirtualMachine;

	class CharacterClass : public Class
	{
	public:
	};

	class CharacterClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};