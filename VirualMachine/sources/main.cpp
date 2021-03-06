#include "stdafx.h"
#include "prereq.h"

#include "Method.h"
#include "VirtualMachine.h"
#include "Debugger.h"
#include "Console.h"
#include "Thread.h"
#include "MyClassFileLoader.h"

#include "PerformanceProfiler.h"

using namespace Beer;

struct Settings
{
	bool run;
	bool loadClassFile;
	bool printClassFile;
	bool printBytecode;
	bool debuggerEnabled;
	bool debuggerStepping;
	bool manualThreads;
	string classFileName;
	uint32 stackSize;
	uint32 heapSize;
	uint32 threadsCount;

	INLINE Settings() :
		printClassFile(false), 
		loadClassFile(true), 
		classFileName(BEER_WIDEN("")), 
		run(true), 
		debuggerEnabled(false), 
		debuggerStepping(false),
		printBytecode(false),
		stackSize(1*1024),
		heapSize(10*1024*1024),
		manualThreads(false),
		threadsCount(0)

	{}
};

bool loadFile(string filename, byte** out_data, uint32& out_length)
{
	std::ifstream f; // *NO* wide!!!
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

bool parseMemorySize(string in_value, uint32& out_value)
{//TODO
	stringstream ss(in_value);
	ss >> out_value;
	return !ss.fail();
}

bool parseInteger(string in_value, uint32& out_value)
{
	stringstream ss(in_value);
	ss >> out_value;
	return !ss.fail();
}

bool loadSettings(int argc, const char** argv, Settings& settings)
{
	if(argc < 2)
	{
		cout << "error: classfile name missing" << std::endl;
		return false;
	}

	stringstream ss;
	ss << argv[1];
	settings.classFileName = ss.str();//argv[1];

	for(int parami = 2; parami < argc; parami++)
	{
		ss.str(BEER_WIDEN(""));
		ss << argv[parami];
		string param(ss.str());

		if(param.find(BEER_WIDEN("--")) > 0) continue; // not a --param

		//cout << param << ":" << param.find(BEER_WIDEN("--")) << std::endl;

		if(param.compare(BEER_WIDEN("--help")) == 0)
		{
			cout << "--help" << std::endl;
			cout << "--run[:false]" << std::endl;
			cout << "--threads:value" << std::endl;
			cout << "--printbytecode[:false]" << std::endl;
			cout << "--printclassfile[:false]" << std::endl;
			cout << "--debugger[:false]" << std::endl;
			cout << "--stepping[:false]" << std::endl;
			//cout << "--loadclassfile[:false]" << std::endl;
			exit(0);
		}
		
		string value;
		string name;
		size_t pos = param.find(':');
		
		name = param.substr(0, pos <= param.size() ? pos : param.size());
		if(name.size() > 0) value = param.substr(name.size());

		if(name.compare(BEER_WIDEN("--run")) == 0)
		{
			settings.run = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
		}
		else if(name.compare(BEER_WIDEN("--threads")) == 0)
		{
			settings.manualThreads = true;

			if(!parseInteger(value.substr(1), settings.threadsCount))
			{
				cout << BEER_WIDEN("Unable to parse threads value: ") << value;
				return false;
			}
		}
		else if(name.compare(BEER_WIDEN("--printclassfile")) == 0)
		{
			settings.printClassFile = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
		}
		else if(name.compare(BEER_WIDEN("--printbytecode")) == 0)
		{
			settings.printBytecode = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
		}
		else if(name.compare(BEER_WIDEN("--loadclassfile")) == 0)
		{
			settings.loadClassFile = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
		}
		else if(name.compare(BEER_WIDEN("--debugger")) == 0)
		{
			settings.debuggerEnabled = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
		}
		else if(name.compare(BEER_WIDEN("--stepping")) == 0)
		{
			settings.debuggerStepping = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
		}
		else if(name.compare(BEER_WIDEN("--stacksize")) == 0)
		{
			if(!parseMemorySize(value.substr(1), settings.stackSize))
			{
				cout << BEER_WIDEN("Unable to parse stacksize value: ") << value;
				return false;
			}
		}
		else if(name.compare(BEER_WIDEN("--heapsize")) == 0)
		{
			if(!parseMemorySize(value.substr(1), settings.heapSize))
			{
				cout << BEER_WIDEN("Unable to parse heapsize value: ") << value;
				return false;
			}
		}
		else
		{
			cout << BEER_WIDEN("Unknown parameter: ") << name;
			return false;
		}
	}

	return true;
}

bool sortMethodByTime(Method* m1, Method* m2)
{
	return m1->getTimeSpent() > m2->getTimeSpent();
}

void printBytecodes(VirtualMachine *vm)
{
	ClassReflectionTable& klasses = vm->getClasses();
	for(ClassReflectionTable::iterator it = klasses.begin(); it != klasses.end(); it++)
	{
		Class* klass = it->second;
		vm->getDebugger()->printBytecodeMethods(klass);
	}
}


int __cdecl main(int argc, const char** argv)
{
	// Memory debugging
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF );
	//_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_WNDW );

	//testInteger();

	// set UTF-16 support
	_setmode( _fileno(stdout), _O_U16TEXT);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	uint32 numberOfProcessors = sysinfo.dwNumberOfProcessors;

	MiliTimer mainTimer;
	mainTimer.start();

	Settings settings;
	if(!loadSettings(argc, argv, settings)) return 1;

	if(true) // TODO: macro BEER_SINGLE_THREADED vs BEER_MULTI_THREADED
	{
		settings.manualThreads = true;
		settings.threadsCount = 1;
	}

	uint32 numberOfThreads = settings.manualThreads ? settings.threadsCount : numberOfProcessors;
	//cout << "Threads: " << numberOfThreads << "\n";

	GenerationalGC* gc = new GenerationalGC(
		     384/*1024*/ * 1024, // nursery size 1MB
		        4 * 1024, // thread private allocation block 4kB
		16 * 1024 * 1024, // mature gen size 16MB
		     1024 * 1024); // permanent gen size 1MB
	
	ClassFileLoader* classFileLoader = new MyClassFileLoader();
	VirtualMachine* vm = new VirtualMachine(gc);
	ClassFileDescriptor* classFile = NULL;

	gc->init(vm);

//	Console::setArgs(&argv[2], argc - 2); // TODO

	try
	{
		vm->init(numberOfThreads);
		vm->getDebugger()->setEnabled(settings.debuggerEnabled);
		vm->getDebugger()->setSteppingMode(settings.debuggerStepping);

		uint32 dataLength = 0;
		byte* data = NULL;

		if(!loadFile(settings.classFileName, &data, dataLength))
		{
			throw IOException(string(BEER_WIDEN("Could not open file: ")) + settings.classFileName);
		}

		classFile = classFileLoader->loadClassFile(data, dataLength);
		
		if(settings.printClassFile) classFileLoader->printClassFile(classFile);
		if(settings.loadClassFile)
		{
			vm->loadClassFile(classFileLoader, classFile);
		}
		if(settings.printBytecode)
		{
			printBytecodes(vm);
		}

		if(settings.run)
		{
			vm->run();
			//vm->wait();
			//Console::getOutput().flush(cout);
		}

		vm->destroy();

		SMART_DELETE_ARR(data);
	}

	catch(Exception& ex)
	{
		//vm->getDebugger()->printCallStack();
		//vm->getDebugger()->printStack();
		cout << std::endl << "Unhandled exception: " << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;
	}

	SMART_DELETE(vm);
	SMART_DELETE(gc);

#ifdef BEER_MEASURE_PERFORMANCE
	Console::getOutput() << BEER_WIDEN("\nMain took ") << mainTimer.stop();
#endif // BEER_MEASURE_PERFORMANCE

#if defined(BEER_PROFILE_MONOCACHES) || defined(BEER_PROFILE_POLYCACHES)
	PerformanceProfiler* profiler = PerformanceProfiler::getInstance();
	cout << "\n";
#endif // 

#ifdef BEER_PROFILE_MONOCACHES
	cout << "MIC waiting: " << std::fixed << std::setw(11) << std::setprecision(8) << profiler->getMonoCachesWaiting() << "\n";
#endif

#ifdef BEER_PROFILE_POLYCACHES
	cout << "PIC waiting: " << std::fixed << std::setw(11) << std::setprecision(8) << profiler->getPolyCachesWaiting() << "\n";
#endif

	//system(BEER_WIDEN("PAUSE"));test
	return 0;
}