#include "stdafx.h"
#include "prereq.h"

#include "MethodReflection.h"
#include "VirtualMachine.h"
#include "Debugger.h"

#include "IntegerClass.h"
#include "StringClass.h"
#include "ConsoleClass.h"

#include "Thread.h"
#include "InterlockedQueue.h"

#include "MyClassFileLoader.h"

using namespace Beer;

struct Settings
{
	bool run;
	bool loadClassFile;
	bool printClassFile;
	bool debuggerEnabled;
	bool debuggerStepping;
	std::string classFileName;

	INLINE Settings() :
		printClassFile(false), 
		loadClassFile(true), 
		classFileName(""), 
		run(true), 
		debuggerEnabled(false), 
		debuggerStepping(false)
	{}
};


bool loadFile(std::string filename, byte** out_data, uint32& out_length)
{
	std::fstream f;
	f.open(filename, std::ios::in|std::ios::binary);

	if(!f.good()) return false;

	f.seekg(0, std::ios::end);
	
	uint32 dataLength = static_cast<uint32>(f.tellg());//.state();
	byte* data = new byte[dataLength];

	f.seekg(0);
	f.read(reinterpret_cast<char*>(data), dataLength);
	f.close();

	*out_data = data;
	out_length = dataLength;
	
	return true;
}

bool loadSettings(int argc, const char** argv, Settings& settings)
{
	if(argc < 2)
	{
		std::cout << "error: classfile name missing" << std::endl;
		return false;
	}

	settings.classFileName = argv[1];

	for(int parami = 2; parami < argc; parami++)
	{
		std::string param(argv[parami]);

		if(param.find("--") > 0) continue; // not a --param

		std::cout << param << ":" << param.find("--") << std::endl;

		if(param.compare("--help") == 0)
		{
			std::cout << "--help" << std::endl;
			std::cout << "--run[:false]" << std::endl;
			std::cout << "--printclassfile[:false]" << std::endl;
			std::cout << "--debugger[:false]" << std::endl;
			std::cout << "--stepping[:false]" << std::endl;
			//std::cout << "--loadclassfile[:false]" << std::endl;
			exit(0);
		}
		
		std::string value;
		std::string name;
		size_t pos = param.find(':');
		
		name = param.substr(0, pos <= param.size() ? pos : param.size());
		if(name.size() > 0) value = param.substr(name.size());

		if(name.compare("--run") == 0)
		{
			settings.run = value.compare(":false") == 0 ? false : true;
		}
		else if(name.compare("--printclassfile") == 0)
		{
			settings.printClassFile = value.compare(":false") == 0 ? false : true;
		}
		else if(name.compare("--loadclassfile") == 0)
		{
			settings.loadClassFile = value.compare(":false") == 0 ? false : true;
		}
		else if(name.compare("--debugger") == 0)
		{
			settings.debuggerEnabled = value.compare(":false") == 0 ? false : true;
		}
		else if(name.compare("--stepping") == 0)
		{
			settings.debuggerStepping = value.compare(":false") == 0 ? false : true;
		}
		else
		{
			std::cout << "Unknown parameter: " + name;
			return false;
		}
	}

	return true;
}

#include "DynamicStack.h"


int __cdecl main(int argc, const char** argv)
{
	//test();
	Settings settings;
	if(!loadSettings(argc, argv, settings)) return 1;

	ClassFileLoader* classFileLoader = new MyClassFileLoader();
	VirtualMachine* vm = new VirtualMachine;
	ClassFileDescriptor* classFile = NULL;

	Console::setArgs(&argv[2], argc - 2); // TODO

	try
	{
		vm->init(1024, 5*1024*1024);//*1024/*50*1024*1024, 750*1024*1024*/);
		vm->getDebugger()->setEnabled(settings.debuggerEnabled);
		vm->getDebugger()->setSteppingMode(settings.debuggerStepping);

		uint32 dataLength = 0;
		byte* data = NULL;

		if(!loadFile(settings.classFileName, &data, dataLength))
		{
			throw IOFileException(std::string("Could not open file: ") + settings.classFileName);
		}

		classFile = classFileLoader->loadClassFile(data, dataLength);
		
		if(settings.printClassFile) classFileLoader->printClassFile(classFile);
		if(settings.loadClassFile) classFileLoader->loadClasses(vm, classFile);
		if(settings.run) vm->run();

		vm->destroy();

		SMART_DELETE_ARR(data);
	}

	catch(Exception& ex)
	{
		//vm->getDebugger()->printCallStack();
		//vm->getDebugger()->printStack();
		std::cout << std::endl << "Unhandled exception: " << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;
	}

	SMART_DELETE(vm);

	//system("PAUSE");
	return 0;
}