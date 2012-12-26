#include "stdafx.h"
#include "AttributeDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"
#include "ClassDescriptor.h"
#include "StringDescriptor.h"

using namespace Beer;


const StringDescriptor* AttributeDescriptor::getName(const ClassFileDescriptor* classFile) const
{
	return classFile->getDescriptor<StringDescriptor>(getNameId());
}

void AttributeDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	//////////// convert my data

	format.convert(getNameId());
	format.convert(getFlags());
	format.convert(getTypeId());

	//////////// convert referenced

	// convert name
	classFile->getDescriptor<StringDescriptor>(getNameId())->convert(format, classFile);

	// no need to convert type
}