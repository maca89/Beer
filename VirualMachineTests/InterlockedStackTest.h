#pragma once
#include "prereq.h"
#include "TestSuite.h"
#include "InterlockedStack.h"


namespace Beer
{
	class InterlockedStackTest : public Test::Suite
	{
	public:
		typedef InterlockedStack<int32> InterlockedStack32;

	protected:
		InterlockedStack32 stack32;

	public:
		InterlockedStackTest();

	protected:
		virtual void setup();
		virtual void tear_down();

		void testPush();
		void testPop();
		void testSimple();
		void testParallel();
	};
};