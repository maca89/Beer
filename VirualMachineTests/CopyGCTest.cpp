#include "stdafx.h"
#include "CopyGCTest.h"
#include "String.h"

using namespace Beer;

string intToString(int32 n)
{
	stringstream ss;
	ss << n;
	return ss.str();
}

namespace Beer
{
	class Foo : public Beer::CollectedObject
	{
		EXTENDING_COLLECTED_OBJECT_ADDING_0();
	};

	class Bar : public Beer::CollectedObject
	{
		EXTENDING_COLLECTED_OBJECT_ADDING_2(CHILD_STRING_1, CHILD_STRING_2);
	};
};


CopyGCTest::CopyGCTest() : gc(NULL)
{
    TEST_ADD(CopyGCTest::test1);
    TEST_ADD(CopyGCTest::test2);
    TEST_ADD(CopyGCTest::test3);
    TEST_ADD(CopyGCTest::test4);
}
	
void CopyGCTest::setup()
{
	stack.clear();
	gc = new CopyGC(&stack);
	//cout << "--------- gc test ---------" << std::endl;
}

void CopyGCTest::tear_down()
{
	SMART_DELETE(gc);
}

String* CopyGCTest::createString(string value)
{
	String* str = static_cast<GarbageCollector*>(gc)->alloc<String>();
	str->setData(value);
	return str;
}

Bar* CopyGCTest::createBar()
{
	StackRef<Bar> bar(&stack, stack.push(static_cast<GarbageCollector*>(gc)->alloc<Bar>()));
	Bar* ptr = bar.get();

	bar->setChild(Bar::CHILD_STRING_1, createString(L"MyString1"));
	bar->setChild(Bar::CHILD_STRING_2, createString(L"MyString2"));

	stack.pop();
	return ptr;
}

Foo* CopyGCTest::createFoo()
{
	return static_cast<GarbageCollector*>(gc)->alloc<Foo>();
}
    
void CopyGCTest::test1()
{
	Foo* foo = static_cast<GarbageCollector*>(gc)->alloc<Foo>();
	gc->collect();

	TEST_ASSERT_MSG(gc->countLastCollected() == 1, BEER_WIDEN("Object should be deleted"));
}
    
void CopyGCTest::test2()
{
	Foo* foo = static_cast<GarbageCollector*>(gc)->alloc<Foo>();
	stack.push(foo);

	gc->collect();
	TEST_ASSERT_MSG(gc->countLastCollected() == 0, BEER_WIDEN("Object should *NOT* be deleted"));
}
    
void CopyGCTest::test3()
{
	Bar* bar = createBar();

	String* str1 = bar->getChild<String>(Bar::CHILD_STRING_1);
	String* str2 = bar->getChild<String>(Bar::CHILD_STRING_2);

	//cout << str1->getData() << ", BEER_WIDEN(" << str2->getData() << std::endl;
	
	gc->collect();
	TEST_ASSERT_MSG(gc->countLastCollected() == 3, ")Object should be deleted");
}
    
void CopyGCTest::test4()
{
	StackRef<Bar> bar(&stack, stack.push(createBar()));
	Bar* b = bar.get();

	StackRef<String> str1(&stack, stack.push(bar->getChild<String>(Bar::CHILD_STRING_1)));
	String* str2 = bar->getChild<String>(Bar::CHILD_STRING_2);

	//cout << str1.get()->getData() << "=" << str1->getData() << ", BEER_WIDEN(" << str2->getData() << std::endl;
	
	gc->collect();
	TEST_ASSERT_MSG(gc->countLastCollected() == 0, (string(L")Object should *NOT* be deleted, were collected: ") + intToString(gc->countLastCollected())).c_str());
}