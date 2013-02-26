#include "stdafx.h"
#include "prereq.h"

#include "Method.h"
#include "VirtualMachine.h"
#include "Debugger.h"

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
	string classFileName;
	uint32 stackSize;
	uint32 heapSize;

	INLINE Settings() :
		printClassFile(false), 
		loadClassFile(true), 
		classFileName(BEER_WIDEN("")), 
		run(true), 
		debuggerEnabled(false), 
		debuggerStepping(false),
		stackSize(1*1024),
		heapSize(10*1024*1024)
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
		else if(name.compare(BEER_WIDEN("--printclassfile")) == 0)
		{
			settings.printClassFile = value.compare(BEER_WIDEN(":false")) == 0 ? false : true;
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


int __cdecl main(int argc, const char** argv)
{
	MiliTimer mainTimer;
	mainTimer.start();

	Settings settings;
	if(!loadSettings(argc, argv, settings)) return 1;

	ClassFileLoader* classFileLoader = new MyClassFileLoader();
	VirtualMachine* vm = new VirtualMachine;
	ClassFileDescriptor* classFile = NULL;

//	Console::setArgs(&argv[2], argc - 2); // TODO

	try
	{
		vm->init(settings.stackSize, settings.heapSize);
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
		if(settings.loadClassFile) classFileLoader->loadClasses(vm, classFile);
		if(settings.run) { vm->run(); vm->wait(); }

	#ifdef BEER_MEASURE_PERFORMANCE
		typedef std::list<Method*> MethodList;
		MethodList methods;

		for(ClassReflectionTable::iterator it = vm->getClasses().begin(); it != vm->getClasses().end(); it++)
		{
			Class* klass = it->second;
			for(uint32 i = 0; i < klass->getMethodsCount(); i++)
			{
				Method* method = klass->getMethod(i);
				if(method)
				{
					methods.push_back(method);
				}
			}
		}

		methods.sort(&sortMethodByTime);

		cout << "\n-------------------\n-   PERFORMANCE   -\n-------------------\n";

		
		uint16 methodi = 0;
		for(MethodList::iterator it = methods.begin(); it != methods.end(); it++)
		{
			MethodReflection* method = *it;
			
			if(methodi > 20) break;
			if(method->getTimeSpent() == 0) break;
			
			cout << std::setprecision(6) << std::fixed << method->getTimeSpent() << BEER_WIDEN("s") << BEER_WIDEN(" in ") << method->getSelector() << "\n";

			methodi++;
		}

		float64 sum = 0;
		for(MethodList::iterator it = methods.begin(); it != methods.end(); it++)
		{
			sum += (*it)->getTimeSpent();
		}

		cout << "sum: " << sum << "\n";

	#endif // BEER_MEASURE_PERFORMANCE

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

#ifdef BEER_MEASURE_PERFORMANCE
	Console::getOutput() << BEER_WIDEN("\nMain took ") << mainTimer.stop();
#endif // BEER_MEASURE_PERFORMANCE

	//system(BEER_WIDEN("PAUSE"));
	return 0;
}