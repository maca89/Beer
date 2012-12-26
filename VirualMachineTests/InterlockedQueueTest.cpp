#include "stdafx.h"
#include "InterlockedQueueTest.h"
#include "Thread.h"

using namespace Beer;


class QueueTask : public Thread
{

protected:
	InterlockedQueueTest::InterlockedQueue32* mQueue;

public:
	QueueTask(InterlockedQueueTest::InterlockedQueue32* queue) : mQueue(queue)
	{
	}

protected:
	virtual void work()
	{
		size_t times = 10;
		for(size_t i = 0; i < times; i++)
		{
			mQueue->push(i);
			sqrt(50);
		}
		for(size_t i = 0; i < times; i++)
		{
			mQueue->pop();
		}
	}
};


InterlockedQueueTest::InterlockedQueueTest()
{
    TEST_ADD(InterlockedQueueTest::testPush);
    TEST_ADD(InterlockedQueueTest::testPop);
    TEST_ADD(InterlockedQueueTest::testSimple);
    TEST_ADD(InterlockedQueueTest::testParallel);
}
	
void InterlockedQueueTest::setup()
{
	queue32 = InterlockedQueue32();
}

void InterlockedQueueTest::tear_down()
{
	// remove resources
}
    
void InterlockedQueueTest::testPush()
{
	TEST_ASSERT(queue32.empty());
	queue32.push(1);
	TEST_ASSERT(!queue32.empty());
}
	
void InterlockedQueueTest::testPop()
{
	TEST_ASSERT(queue32.empty());
	queue32.push(42);
	TEST_ASSERT(!queue32.empty());
	TEST_ASSERT(queue32.pop() == 42);
	TEST_ASSERT(queue32.empty());
}

void InterlockedQueueTest::testSimple()
{
	InterlockedQueue32 work;
	TEST_ASSERT(work.empty());
		
	int nums[6] = {5, 1, 2, 8, 3, 9};

	for(size_t i = 0; i < 6; i++)
	{
		work.push(nums[i]);
		TEST_ASSERT(!work.empty());
	}

	int nums2[6];
	size_t i = 0;
	while(!work.empty())
	{
		nums2[i] = work.pop();
		i++;
	}

	TEST_ASSERT(work.empty());

	for(size_t i = 0; i < 6; i++)
	{
		TEST_ASSERT(nums[i] == nums2[i]);
	}

	work.push(42);
	TEST_ASSERT(!work.empty());
}

void InterlockedQueueTest::testParallel()
{
	InterlockedQueue32 work;
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



//TEST_ADD_SUITE(InterlockedQueueTest)