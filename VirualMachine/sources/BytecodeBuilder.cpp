#include "stdafx.h"
#include "BytecodeBuilder.h"

using namespace Beer;


GrowableMemoryAllocator BytecodeBuilder::gMemoryAllocator = GrowableMemoryAllocator();
void* BytecodeBuilder::gMemory = NULL;