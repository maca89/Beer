#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <map>
#include <stack>
#include <queue>
#include <set>
#include <list>
#include <time.h>
#include <fstream>

// windows
#include <windows.h>

#ifdef _MSC_VER
//	#pragma warning (disable: 4290)
#	pragma warning (disable: 4099)
#endif

// CppTest
#include "../cpptest/cpptest.h"

#ifdef _DEBUG
#	pragma comment(lib, "../bin/VirtualMachine_d.lib")
#	pragma comment(lib, "../libs/cpptest_d.lib")
#else
#	pragma comment(lib, "../bin/VirtualMachine.lib")
#	pragma comment(lib, "../libs/cpptest.lib")
#endif // _DEBUG