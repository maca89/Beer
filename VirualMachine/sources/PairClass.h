#pragma once
#include "prereq.h"
#include "Pair.h"
#include "Class.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "IntegerClass.h"

namespace Beer
{
	class VirtualMachine;

	class PairClass : public Class
	{
	};

	class PairClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
