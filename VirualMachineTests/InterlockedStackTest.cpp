#include "stdafx.h"
#include "InterlockedStackTest.h"
#include "Thread.h"

using namespace Beer;


class QueueTask : public Thread
{

protected:
	InterlockedStackTest::InterlockedStack32* mStack;

public:
	QueueTask(InterlockedStackTest::InterlockedStack32* stack) : mStack(stack)
	{
	}

protected:
	virtual void work()
	{
		size_t times = 10;
		for(size_t i = 0; i < times; i++)
		{
			mStack->push(i);
			sqrt(50);
		}
		for(size_t i = 0; i < times; i++)
		{
			mStack->pop();
		}
	}
};


InterlockedStackTest::InterlockedStackTest()
{
    TEST_ADD(InterlockedStackTest::testPush);
    TEST_ADD(InterlockedStackTest::testPop);
    TEST_ADD(InterlockedStackTest::testSimple);
    TEST_ADD(InterlockedStackTest::testParallel);
}
	
void InterlockedStackTest::setup()
{
	stack32 = InterlockedStack32();
}

void InterlockedStackTest::tear_down()
{
	//stack32.clear();
}
    
void InterlockedStackTest::testPush()
{
	TEST_ASSERT(stack32.empty());
	stack32.push(1);
	TEST_ASSERT(!stack32.empty());
}
	
void InterlockedStackTest::testPop()
{
	TEST_ASSERT(stack32.empty());
	stack32.push(42);
	TEST_ASSERT(!stack32.empty());
	TEST_ASSERT(stack32.pop() == 42);
	TEST_ASSERT(stack32.empty());
}

void InterlockedStackTest::testSimple()
{
	TEST_ASSERT(stack32.empty());
		
	int nums[6] = {5, 1, 2, 8, 3, 9};

	for(size_t i = 0; i < 6; i++)
	{
		stack32.push(nums[i]);
		TEST_ASSERT(!stack32.empty());
	}

	int nums2[6];
	size_t i = 0;
	while(!stack32.empty())
	{
		TEST_ASSERT(i < 6);
		if(i >= 6) break;
		nums2[i] = stack32.pop();
		i++;
	}
	TEST_ASSERT(i == 6);

	TEST_ASSERT(stack32.empty());

	for(i = 0; i < 6; i++)
	{
		TEST_ASSERT(nums[i] == nums2[6 - i - 1]);
	}

	stack32.push(42);
	TEST_ASSERT(!stack32.empty());
}

void InterlockedStackTest::testParallel()
{
	InterlockedStack32 work;
	TEST_ASSERT(work.empty());

	size_t threadsCount = 50;
	QueueTask** tasks = new QueueTask*[threadsCount];
	
	for(size_t i = 0; i < threadsCount; i++)
	{
		tasks[i] = new QueueTask(&work);
	}

	Sleep(100);
	
	for(size_t i = 0; i < threadsCount; i++)
	{
		tasks[i]->run();
	}

	for(size_t i = 0; i < threadsCount; i++)
	{
		tasks[i]->wait();
	}

	TEST_ASSERT(work.empty());
}
