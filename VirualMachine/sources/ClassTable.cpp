#include "stdafx.h"
#include "ClassTable.h"

using namespace Beer;


uint32 ClassTable::add(ClassReflection* klass)
{
	// ...01
	if(mNext == 1)
	{
		for(; mNext < TABLE_SIZE; mNext += 4)
		{
			mTable[mNext] = klass;
		}
		mNext = 3;
		return 1;
	}

	// ...11
	else
	{
		mTable[mNext] = klass;
		mNext += 4;
		return mNext - 4;
	}
}