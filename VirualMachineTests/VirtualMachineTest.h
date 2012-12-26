#pragma once
#include "prereq.h"
#include "TestSuite.h"
#include "VirtualMachine.h"


namespace Beer
{
	class VirtualMachineTest : public Test::Suite
	{
	public:

	protected:
		VirtualMachine* vm;

	public:
		VirtualMachineTest();

	protected:
		virtual void setup();
		virtual void tear_down();

		void test1();
		void test2();
	};
};