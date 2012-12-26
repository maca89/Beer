#pragma once
#include "prereq.h"
#include "TestSuite.h"
#include "InterlockedQueue.h"


namespace Beer
{
	class InterlockedQueueTest : public Test::Suite
	{
	public:
		typedef InterlockedQueue<int32> InterlockedQueue32;

	protected:
		InterlockedQueue32 queue32;

	public:
		InterlockedQueueTest();

	protected:
		virtual void setup();
		virtual void tear_down();

		void testPush();
		void testPop();
		void testSimple();
		void testParallel();
	};
};