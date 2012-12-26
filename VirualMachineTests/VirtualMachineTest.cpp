#include "stdafx.h"
#include "VirtualMachineTest.h"

#include "ClassLoader.h"
#include "ObjectClass.h"
#include "MethodReflection.h"
#include "Bytecode.h"

using namespace Beer;


class Main : public Object
{
	EXTENDING_COLLECTED_OBJECT_ADDING_0();

public:

};


class MainClass : public ClassReflection
{
public:
	virtual Object* createInstance(GarbageCollector* gc)
	{
		Main* main = gc->alloc<Main>();
		main->setClass(this);
		return main;
	}
};

class MainClassInitializer : public ClassInitializer
{
protected:
	Bytecode* mBc;

public:
	MainClassInitializer(Bytecode* bc) : mBc(bc)
	{
	}

	// interface ClassInitializer
	virtual void createClass(VirtualMachine* vm, ClassLoader* classLoader, std::string name)
	{
	}
	
	virtual void initClass(VirtualMachine* vm, ClassReflection* klass)
	{
		klass->extends(0, vm->getClass("Object"));

		BytecodeMethodReflection* methodRun = new BytecodeMethodReflection;
		methodRun->setSelector(klass->getName() + "::run()");
		methodRun->setBytecode(mBc);
		klass->addMethod(0, methodRun);
	}
};


VirtualMachineTest::VirtualMachineTest() : vm(NULL)
{
    //TEST_ADD(VirtualMachineTest::test1);
    TEST_ADD(VirtualMachineTest::test2);
}
	
void VirtualMachineTest::setup()
{
	vm = new VirtualMachine;
}

void VirtualMachineTest::tear_down()
{
	SMART_DELETE(vm);
}

void VirtualMachineTest::test1()
{
	try
	{
		vm->init();

		{
			size_t instrsLength = 128;
			byte instrs[128];
			ZeroMemory(instrs, instrsLength * sizeof(byte));
			size_t i = 0;

			instrs[i++] = Bytecode::INSTR_PUSH_STRING;

			vm->getClassLoader()->addClassInitializer("Main", new MainClassInitializer(new Bytecode(instrs, instrsLength)));
			vm->getClassLoader()->createClass<MainClass>("Main", 1, 0, 1);

			Main* main = vm->getClass("Main")->createInstance<Main>(vm->getHeap());
			
			//main->call(vm, MainClass::Methods::RUN_VOID);
		}

		// init vars
		/*Reference consoleInstance = getConsoleClass()->createInstance();
		Reference textInstance = getStringClass()->createInstance("a = ");
		Reference aInstance = getIntegerClass()->createInstance(5);

		// print text
		getStack()->push(textInstance); //push text
		consoleInstance->getClass()->getMethod(ConsoleClass::Methods::PRINT_STRING)->call(this, consoleInstance);
		getStack()->pop(); // pop text

		// print a
		getStack()->push(aInstance); //push a
		consoleInstance->getClass()->getMethod(ConsoleClass::Methods::PRINT_INTEGER)->call(this, consoleInstance);
		getStack()->pop(); // pop a

		// print new line
		consoleInstance->getClass()->getMethod(ConsoleClass::Methods::PRINTLN_VOID)->call(this, consoleInstance);*/


		vm->run();
		vm->destroy();
	}

	catch(RuntimeAssertException& ex)
	{
		std::cout << ex.getFilename() << "(" << ex.getFileline() << ") : " << ex.getMessage() << std::endl;
		//if(!vm->getStack()->empty()) vm->printStack();
		//if(!Instance::gPool.empty()) vm->printInstances();
		TEST_FAIL("RuntimeAssertException");
	}

	catch(Exception& ex)
	{
		std::cout << ex.getFilename() << ":" << ex.getFileline() << " " << ex.getMessage() << std::endl;
		//if(!vm->getStack()->empty()) vm->printStack();
		//if(!Instance::gPool.empty()) vm->printInstances();
		TEST_FAIL("Exception");
	}
}

void VirtualMachineTest::test2()
{
	std::cout << std::endl;
	try
	{
		vm->init();
	
		std::fstream f;
		f.open("data/program.beer", std::ios::in|std::ios::binary);

		TEST_ASSERT_MSG(f.good(), "Could not open file");

		f.seekg(0, std::ios::end);
	
		uint32 dataLength = static_cast<uint32>(f.tellg());//.state();
		byte* data = new byte[dataLength];

		f.seekg(0);
		f.read(reinterpret_cast<char*>(data), dataLength);
		f.close();

		vm->loadClassFile(data, dataLength);

		//vm->run();
		vm->destroy();

		SMART_DELETE_ARR(data);
	}

	catch(RuntimeAssertException& ex)
	{
		std::cout << ex.getFilename() << "(" << ex.getFileline() << ") : " << ex.getMessage() << std::endl;
		//if(!vm->getStack()->empty()) vm->printStack();
		//if(!Instance::gPool.empty()) vm->printInstances();
		TEST_FAIL("RuntimeAssertException");
	}

	catch(Exception& ex)
	{
		std::cout << ex.getFilename() << ":" << ex.getFileline() << " " << ex.getMessage() << std::endl;
		//if(!vm->getStack()->empty()) vm->printStack();
		//if(!Instance::gPool.empty()) vm->printInstances();
		TEST_FAIL("Exception");
	}
}