#include "stdafx.h"
#include "ClassFileDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassDescriptor.h"
#include "StringDescriptor.h"

using namespace Beer;


const StringDescriptor* ClassFileDescriptor::getClassName(uint16 i) const
{
	int16 classId = getClassId(i);
	
	if(isExternalClass(i))
	{
		return getDescriptor<StringDescriptor>(classId);
	}
	else
	{
		return getDescriptor<ClassDescriptor>(classId)->getName(this);
	}
}

void ClassFileDescriptor::convert(FileFormatConverter& format)
{
	////////// convert my data

	format.convert(mVersion);
			
	format.convert(getDescriptorsLength());

	for(uint16 i = 0; i < getDescriptorsLength(); i++)
	{
		format.convert(getDescriptorAddr(i));
	}

	format.convert(getExternalClassesLength());
	format.convert(getDefinedClassesLength());

	for(uint16 i = 0; i < getExternalClassesLength() + getDefinedClassesLength(); i++)
	{
		format.convert(getClassId(i));
	}

	////////// convert referenced descriptors

	for(uint16 i = 0; i < getExternalClassesLength(); i++)
	{
		StringDescriptor* stringDescr = getDescriptor<StringDescriptor>(getClassId(i));
		stringDescr->convert(format, this);
	}

	for(uint16 i = 0; i < getDefinedClassesLength(); i++)
	{
		ClassDescriptor* classDescr = getDescriptor<ClassDescriptor>(getClassId(getExternalClassesLength() + i));
		
		classDescr->convert(format, this);
	}
}