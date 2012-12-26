#pragma once
#include "prereq.h"


namespace Beer
{
	class TestSuite : protected Test::Suite
	{
	protected:
		size_t mTestsCount;

	public:
		static TestSuite* All;
		static TestSuite* Single;

		size_t testCount() const
		{
			return mTestsCount;
		}

		bool run(Test::Output& output, bool cont_after_fail = true)
		{
			return this->Test::Suite::run(output, cont_after_fail);
		}

		void add(std::auto_ptr<Test::Suite> ts)
		{
			mTestsCount++;
			this->Test::Suite::add(ts);
		}

	protected:
		TestSuite() : mTestsCount(0) {}
	};
}

#define TEST_ADD_SUITE(_class_) STATIC_INITIALIZER(_class_, {Beer::TestSuite::All->add(std::auto_ptr<Test::Suite>(new (_class_)));})


/*
TEST_FAIL(msg)

TEST_ASSERT(expr)
TEST_ASSERT_MSG(expr, msg)
TEST_ASSERT_DELTA(a, b, delta)
TEST_ASSERT_DELTA_MSG(a, b, delta, msg)

TEST_THROWS(expr, x)
TEST_THROWS_MSG(expr, x, msg)
TEST_THROWS_ANYTHING(expr)
TEST_THROWS_ANYTHING_MSG(expr, msg)
TEST_THROWS_NOTHING(expr)
TEST_THROWS_NOTHING_MSG(expr, msg)
*/

/*TEST_FAIL(msg) TEST_FAIL(__FUNCTION__ ## " -- " ## msg)

TEST_ASSERT(expr)
TEST_ASSERT_MSG(expr, msg)
TEST_ASSERT_DELTA(a, b, delta)
TEST_ASSERT_DELTA_MSG(a, b, delta, msg)

TEST_THROWS(expr, x)
TEST_THROWS_MSG(expr, x, msg)
TEST_THROWS_ANYTHING(expr)
TEST_THROWS_ANYTHING_MSG(expr, msg)
TEST_THROWS_NOTHING(expr)
TEST_THROWS_NOTHING_MSG(expr, msg)*/