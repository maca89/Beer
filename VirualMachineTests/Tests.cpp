#include "stdafx.h"
#include "TestSuite.h"
#include "Timer.h"

#include "InterlockedQueueTest.h"
#include "InterlockedStackTest.h"
#include "QuickSortTest.h"
#include "VirtualMachineTest.h"
#include "MarkAndSweepGCTest.h"
#include "CopyGCTest.h"

using namespace Beer;


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

class ExampleTestSuite : public Test::Suite
{
public:
    ExampleTestSuite()
    {
        TEST_ADD(ExampleTestSuite::first_test)
        TEST_ADD(ExampleTestSuite::second_test)
    }

protected:
    virtual void setup()
	{
		// setup resources
	}

    virtual void tear_down()
	{
		// remove resources
	}
    
private:
    void first_test()
	{
	}
	
	void second_test()
	{
	}
};


void appendTests(Beer::TestSuite* ts)
{
	ts->add(std::auto_ptr<Test::Suite>(new InterlockedQueueTest));
	ts->add(std::auto_ptr<Test::Suite>(new InterlockedStackTest));
	ts->add(std::auto_ptr<Test::Suite>(new QuickSortTest));
	ts->add(std::auto_ptr<Test::Suite>(new VirtualMachineTest));
	//ts->add(std::auto_ptr<Test::Suite>(new MarkAndSweepGCTest));
	ts->add(std::auto_ptr<Test::Suite>(new CopyGCTest));
}




bool htmlOutput(Beer::TestSuite* ts)
{
	std::auto_ptr<Test::HtmlOutput> output(new Test::HtmlOutput());
	bool succ = ts->run(*output, false);

	std::ofstream file(L"VirtualMachineTests.html", std::ios_base::out);
	output->generate(file, true, BEER_WIDEN("VirtualMachineTests"));
	file.close();

	char_t_t buffer[255];
	size_t length = GetCurrentDirectory(255, reinterpret_cast<LPWSTR>(buffer));
	string dir(buffer, length);
	dir += L"/VirtualMachineTests.html";

	if(32 >= (int)ShellExecute(NULL, L"open", dir.c_str(), NULL, NULL, SW_SHOWNORMAL))
	{
		std::wcout << L"Unable to open a file: " << dir << std::endl;
	}
	else
	{
		//cout << "OK" << std::endl;
	}

	return succ;
}

bool textOutput(Beer::TestSuite* ts)
{
	std::auto_ptr<Test::Output> output(new Test::TextOutput(Test::TextOutput::Verbose));
	bool succ = ts->run(*output, false);
	//system(L"PAUSE");
	return succ;
}

bool gccOutput(Beer::TestSuite* ts)
{
	std::auto_ptr<Test::Output> output(new Test::CompilerOutput(Test::CompilerOutput::GCC));
	if(ts->run(*output, false)) { cout << "true" << std::endl; return true; }
	cout << "false" << std::endl;
	return false;
}

bool msvcOutput(Beer::TestSuite* ts)
{
	std::auto_ptr<Test::Output> output(new Test::CompilerOutput(Test::CompilerOutput::MSVC));

	MiliTimer t;
	t.start();

	if(ts->run(*output, false))
	{
		cout << "Tests succeded in " << t.stop() << std::endl;
		return true;
	}
	else
	{
		cout << "Tests failed in " << t.stop() << std::endl;
		return false;
	}
}

int main(int argc, char_t* argv[])
{
	typedef bool (*outputFn)(Beer::TestSuite*);
	typedef std::map<string, outputFn> OuputMap;

	OuputMap outputs;
	outputs["text"] = &textOutput;
	outputs["html"] = &htmlOutput;
	outputs["gcc"] = &gccOutput;
	outputs["msvc"] = &msvcOutput;
	outputFn output = NULL;

	if(argc >= 2)
	{
		string arg = argv[1];
		
		size_t start = arg.find(L"--");
		if(start != 0) { cout << "Unknown parameter (doesnt start with '--')" << std::endl; return EXIT_FAILURE; }
		
		string name = arg.substr(2);
		OuputMap::iterator it = outputs.find(name);

		if(it == outputs.end()) { cout << "Unknown parameter ('" << name << "')" << std::endl; return EXIT_FAILURE; }

		output = it->second;
	}
	else
	{
		output = textOutput;
	}

	appendTests(Beer::TestSuite::All);

	//cout << "Running Unit tests" << std::endl;

	Beer::TestSuite* ts = NULL;
	bool succ = false;

	if(Beer::TestSuite::Single->testCount() > 0)
	{
		ts = Beer::TestSuite::Single;
	}
	else
	{
		ts = Beer::TestSuite::All;
	}

	return output(ts) ? EXIT_SUCCESS : EXIT_FAILURE;
}

