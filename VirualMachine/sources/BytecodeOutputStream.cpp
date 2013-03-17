#include "stdafx.h"
#include "BytecodeOutputStream.h"

using namespace Beer;


GrowableMemoryAllocator BytecodeOutputStream::gMemoryAllocator = GrowableMemoryAllocator();
void* BytecodeOutputStream::gMemory = NULL;