#include "stdafx.h"
#include "StringDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"

using namespace Beer;


void StringDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	if(isMachineEncoded())
	{
		return;
	}

	// no need to convert flags

	// convert size
	format.convert(mSize);
	
	// convert data
	for(uint16 i = 0; i < getSize(); i++)
	{
		format.convert(reinterpret_cast<uint16&>((&mData)[i]));
	}

	markAsMachineEncoded();
}