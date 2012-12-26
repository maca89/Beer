#include "stdafx.h"
#include "MarkAndSweepGCTest.h"

using namespace Beer;


class Foo : public Beer::CollectedObject
{
public:
	bool& deleted;
	Foo(bool& deleted) : deleted(deleted)
	{
		std::cout << "Foo" << std::endl;
	}

	virtual ~Foo()
	{
		std::cout << "~Foo" << std::endl;
		deleted = true;
	}
};


MarkAndSweepGCTest::MarkAndSweepGCTest() : gc(NULL)
{
    TEST_ADD(MarkAndSweepGCTest::test1);
    TEST_ADD(MarkAndSweepGCTest::test2);
    TEST_ADD(MarkAndSweepGCTest::test3);
    TEST_ADD(MarkAndSweepGCTest::test4);
}
	
void MarkAndSweepGCTest::setup()
{
	gc = new MarkAndSweepGC;
}

void MarkAndSweepGCTest::tear_down()
{
	SMART_DELETE(gc);
}
    
void MarkAndSweepGCTest::test1()
{
	bool deleted = false;
	
	/*Reference<*/Foo* /*>*/ foo = new(gc) Foo(deleted);
	gc->free(foo); // manual delete

	TEST_ASSERT_MSG(deleted, "Object should be deleted");
}
    
void MarkAndSweepGCTest::test2()
{
	bool deleted = false;
	
	// block important!
	{
		Foo* foo = new(gc) Foo(deleted);
	}
	
	gc->collect(); // automatic delete
	TEST_ASSERT_MSG(deleted, "Object should be deleted");
}
    
void MarkAndSweepGCTest::test3()
{
	bool deleted = false;
	
	// block important!
	Foo* foo = new(gc) Foo(deleted);
	gc->collect();
	
	TEST_ASSERT_MSG(!deleted, "Object should not be deleted - still in use");
}
    
void MarkAndSweepGCTest::test4()
{
}