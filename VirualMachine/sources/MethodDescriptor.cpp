#include "stdafx.h"
#include "MethodDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "ParamDescriptor.h"
#include "BytecodeDescriptor.h"

using namespace Beer;


const StringDescriptor* MethodDescriptor::getName(ClassFileDescriptor* classFile) const
{
	return classFile->getDescriptor<StringDescriptor>(getNameId());
}

void MethodDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	//////////// convert my data
	
	format.convert(getFlags());
	format.convert(getNameId());

	format.convert(getParamsLength());
	for(uint16 i = 0; i < getParamsLength(); i++)
	{
		format.convert(getParamId(i));
	}

	format.convert(getReturnsLength());
	for(uint16 i = 0; i < getReturnsLength(); i++)
	{
		format.convert(getReturnId(i));
	}

	format.convert(getBytecodeId());

	//////////// convert referenced

	// convert name
	classFile->getDescriptor<StringDescriptor>(getNameId())->convert(format, classFile);

	// convert params
	for(uint16 i = 0; i < getParamsLength(); i++)
	{
		classFile->getDescriptor<ParamDescriptor>(getParamId(i))->convert(format, classFile);
	}

	// convert returns
	for(uint16 i = 0; i < getReturnsLength(); i++)
	{
		classFile->getDescriptor<ParamDescriptor>(getReturnId(i))->convert(format, classFile);
	}

	// convert bytecode
	classFile->getDescriptor<BytecodeDescriptor>(getBytecodeId())->convert(format, classFile);
}