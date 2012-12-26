#include "stdafx.h"
#include "QuickSortTest.h"
#include "QuickSort.h"
#include "InterlockedStack.h"
#include "Thread.h"

using namespace Beer;


void randomize(int32* items, size_t size)
{
	srand(static_cast<unsigned int>(time(NULL)));
	for(size_t i = 0; i < size; i++)
	{
		items[i] = rand();
	}
}

bool isSorted(int32* items, size_t length)
{
	int32 last = items[0];
	for(size_t i = 0; i < length; i++)
	{
		if(last > items[i]) return false;
		last = items[i];
	}

	return true;
}

void print(int32* items, size_t length)
{
	std::cout << "[";
	for(size_t i = 0; i < length; i++)
	{
		std::cout << items[i];
		if(i + 1 < length) std::cout << ", ";
	}
	std::cout << "]" << std::endl;
}


QuickSortTest::QuickSortTest()
{
    TEST_ADD(QuickSortTest::testRecursiveSmall1);
    TEST_ADD(QuickSortTest::testRecursiveSmall2);
    TEST_ADD(QuickSortTest::testRecursiveSmall3);
    TEST_ADD(QuickSortTest::testRecursiveRandom1);
    TEST_ADD(QuickSortTest::testRecursiveRandom2);

    TEST_ADD(QuickSortTest::testIterativeSmall1);
    TEST_ADD(QuickSortTest::testIterativeSmall2);
    TEST_ADD(QuickSortTest::testIterativeSmall3);
    TEST_ADD(QuickSortTest::testIterativeRandomSmall);
    TEST_ADD(QuickSortTest::testIterativeRandomMedium);
    TEST_ADD(QuickSortTest::testIterativeRandomBig);
    
	TEST_ADD(QuickSortTest::testDefaultSmall1);

	TEST_ADD(QuickSortTest::testParallelSmall);
	TEST_ADD(QuickSortTest::testParallelMedium);
	TEST_ADD(QuickSortTest::testParallelBig);
	//TEST_ADD(QuickSortTest::testParallelLarge);
}
	
    
void QuickSortTest::testRecursiveSmall1()
{
	int32 items[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	Beer::QuickSort::resursive(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testRecursiveSmall2()
{
	int32 items[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	Beer::QuickSort::resursive(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testRecursiveSmall3()
{
	int32 items[10] = {5, 6, 8, 3, 4, 1, 2, 1, 2, 2};
	Beer::QuickSort::resursive(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testRecursiveRandom1()
{
	int32 items[100];
	randomize(items, lengthof(items));

	//print(items, lengthof(items));
	Beer::QuickSort::resursive(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testRecursiveRandom2()
{
	size_t length = 1000;
	int32* items = new int32[length];
	randomize(items, lengthof(items));

	Beer::QuickSort::resursive(items, length);
	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}

// non recursive

void QuickSortTest::testIterativeSmall1()
{
	int32 items[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	//print(items, lengthof(items));
	Beer::QuickSort::iterative(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testIterativeSmall2()
{
	int32 items[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	Beer::QuickSort::iterative(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testIterativeSmall3()
{
	int32 items[10] = {5, 6, 8, 3, 4, 1, 2, 1, 2, 2};
	Beer::QuickSort::iterative(items, lengthof(items));
	//print(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testIterativeRandomSmall()
{
	int32 items[100];
	randomize(items, lengthof(items));

	Beer::QuickSort::iterative(items, lengthof(items));
	TEST_ASSERT(isSorted(items, lengthof(items)));
}

void QuickSortTest::testIterativeRandomMedium()
{
	size_t length = 1000;
	int32* items = new int32[length];
	randomize(items, length);

	Beer::QuickSort::iterative(items, length);
	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}

void QuickSortTest::testIterativeRandomBig()
{
	size_t length = 10000;
	int32* items = new int32[length];
	randomize(items, length);
	
	timer.start();
	Beer::QuickSort::iterative(items, length);
	//std::cout << "QuickSort::nonresursive " << length << " in " << timer.stop() << std::endl;

	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}

// default

void QuickSortTest::testDefaultSmall1()
{
	size_t length = 1000;
	int32* items = new int32[length];
	randomize(items, length);

	Beer::QuickSort(items, length);

	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}

// parallel

typedef InterlockedStack<QuickSort::Instance> InstanceStack;
//typedef std::stack<QuickSort::Instance> InstanceStack;

template <typename T>
class QuickSortThread : public Thread
{
protected:
	InstanceStack* mWork;

public:
	QuickSortThread(InstanceStack* work) : mWork(work)
	{
	}
	
//protected:
	// Thread
	virtual void work()
	{
		//QuickSort::Instance inst = mWork->dequeue();
		//QuickSort::iterative(reinterpret_cast<T*>(inst.arr), inst.length);
		//return;

		while(!mWork->empty())
		{
			QuickSort::Instance inst = mWork->top();
			mWork->pop();

			QuickSort::Instance lesser, bigger;
			if(!QuickSort::iterative_step<T>(inst, lesser, bigger))
			{
				continue;
			}

			mWork->push(lesser);
			mWork->push(bigger);
		}
	}
};

template <typename T>
void QuickSort_parallel(T* arr, size_t length)
{
	InstanceStack* work = new InstanceStack;
	work->push(QuickSort::Instance(arr, length));

	/*QuickSortThread<T> t(work);
	t.run();
	//t.work();
	t.wait();*/

	size_t i = 0;
	size_t threadsCount = 1;
	QuickSortThread<T>** threads = new QuickSortThread<T>*[threadsCount];

	for(size_t i = 0; i < threadsCount; i++)
	{
		threads[i] = new QuickSortThread<T>(work);
	}

	for(size_t i = 0; i < threadsCount; i++)
	{
		threads[i]->run();
	}

	for(size_t i = 0; i < threadsCount; i++)
	{
		threads[i]->wait();
	}

	SMART_DELETE(work);
}

void QuickSortTest::testParallelSmall()
{
	size_t length = 100;
	int32* items = new int32[length];
	
	for(size_t i = 0; i < 10; i++) // ten repeats
	{
		randomize(items, length);
		QuickSort_parallel(items, length);
		TEST_ASSERT(isSorted(items, length));
	}
	
	SMART_DELETE_ARR(items);
}

void QuickSortTest::testParallelMedium()
{
	size_t length = 1000;
	int32* items = new int32[length];
	randomize(items, length);

	QuickSort_parallel(items, length);

	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}

void QuickSortTest::testParallelBig()
{
	size_t length = 10000;
	int32* items = new int32[length];
	randomize(items, length);

	timer.start();
	QuickSort_parallel(items, length);
	//std::cout << "QuickSort::parallel " << length << " in " << timer.stop() << std::endl;
	//print(items, length);

	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}

void QuickSortTest::testParallelLarge()
{
	size_t length = 100000;
	int32* items = new int32[length];
	randomize(items, length);

	timer.start();
	QuickSort_parallel(items, length);
	//std::cout << "QuickSort::parallel " << length << " in " << timer.stop() << std::endl;
	//print(items, length);

	TEST_ASSERT(isSorted(items, length));
	SMART_DELETE_ARR(items);
}


//TEST_ADD_SUITE(QuickSortTest)