#pragma once
#include "prereq.h"
#include "TestSuite.h"
#include "Timer.h"


namespace Beer
{
	class QuickSortTest : public Test::Suite
	{
	protected:
		MiliTimer timer;

	public:
		QuickSortTest();

	protected:

		// recursive
		void testRecursiveSmall1();
		void testRecursiveSmall2();
		void testRecursiveSmall3();
		void testRecursiveRandom1();
		void testRecursiveRandom2();

		// iterative
		void testIterativeSmall1();
		void testIterativeSmall2();
		void testIterativeSmall3();
		void testIterativeRandomSmall();
		void testIterativeRandomMedium();
		void testIterativeRandomBig();

		// default
		void testDefaultSmall1();

		// parallel
		void testParallelSmall();
		void testParallelMedium();
		void testParallelBig();
		void testParallelLarge();
	};
};