#pragma once
#include "prereq.h"
#include "TestSuite.h"
#include "GarbageCollector.h"
#include "FixedStack.h"
#include "CollectedObject.h"
#include "CopyGC.h"


namespace Beer
{
	class String;
	class Foo;
	class Bar;

	class CopyGCTest : public Test::Suite
	{
	protected:
		FixedStack<CollectedObject*> stack;
		CopyGC* gc;

	public:
		CopyGCTest();

	protected:
		virtual void setup();
		virtual void tear_down();
	
		void test1();
		void test2();
		void test3();
		void test4();

		String* createString(std::string value);
		Bar* createBar();
		Foo* createFoo();
	};
};