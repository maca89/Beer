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

const StringDescriptor* MethodDescriptor::getInterfaceName(ClassFileDescriptor* classFile) const
{
	uint16 classId = getInterfaceId();
	return classFile->getClassName(classId);
}

void MethodDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	//////////// convert my data
	
	// no need to convert flags
	format.convert(getNameId());
	format.convert(getInterfaceId());
	format.convert(getMaxStack());

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

	if(!isAbstract())
	{
		format.convert(getBytecodeId());
	}

	//////////// convert referenced

	// convert name
	classFile->getDescriptor<StringDescriptor>(getNameId())->convert(format, classFile);

	// no need to convert interface

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
	if(!isAbstract() && !isNative())
	{
		classFile->getDescriptor<BytecodeDescriptor>(getBytecodeId())->convert(format, classFile);
	}
}