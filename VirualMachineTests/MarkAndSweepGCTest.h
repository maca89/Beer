#pragma once
#include "prereq.h"
#include "TestSuite.h"
#include "MarkAndSweepGC.h"


namespace Beer
{
	class MarkAndSweepGCTest : public Test::Suite
	{
	public:

	protected:
		MarkAndSweepGC* gc;

	public:
		MarkAndSweepGCTest();

	protected:
		virtual void setup();
		virtual void tear_down();
	
		void test1();
		void test2();
		void test3();
		void test4();
	};
};