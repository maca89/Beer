#include "stdafx.h"
#include "ClassDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "AttributeDescriptor.h"
#include "MethodDescriptor.h"

using namespace Beer;

const StringDescriptor* ClassDescriptor::getName(const ClassFileDescriptor* classFile) const
{
	return classFile->getDescriptor<StringDescriptor>(getNameId());
}


void ClassDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	/////////////// convert my data

	format.convert(getFlags());
	format.convert(getNameId());
			
	format.convert(getParentsLength());

	for(uint16 i = 0; i < getParentsLength(); i++)
	{
		format.convert(getParentId(i));
	}
			
	format.convert(getAttributesLength());

	for(uint16 i = 0; i < getAttributesLength(); i++)
	{
		format.convert(getAttributeId(i));
	}
			
	format.convert(getMethodsLength());

	for(uint16 i = 0; i < getMethodsLength(); i++)
	{
		format.convert(getMethodId(i));
	}

	/////////////// convert referenced descriptors

	// convert name
	classFile->getDescriptor<StringDescriptor>(getNameId())->convert(format, classFile);

	// no need to convert parents

	// convert attributes
	for(uint16 i = 0; i < getAttributesLength(); i++)
	{
		classFile->getDescriptor<AttributeDescriptor>(getAttributeId(i))->convert(format, classFile);
	}
	
	// convert methods
	for(uint16 i = 0; i < getMethodsLength(); i++)
	{
		classFile->getDescriptor<MethodDescriptor>(getMethodId(i))->convert(format, classFile);
	}


}