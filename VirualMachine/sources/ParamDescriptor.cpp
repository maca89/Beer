#include "stdafx.h"
#include "ParamDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "ClassDescriptor.h"

using namespace Beer;


const StringDescriptor* ParamDescriptor::getName(const ClassFileDescriptor* classFile) const
{
	return classFile->getDescriptor<StringDescriptor>(getNameId());
}

void ParamDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	//////////// convert my data

	format.convert(getNameId());
	// no need to convert flags
	format.convert(getTypeId());

	//////////// convert referenced

	// convert name
	classFile->getDescriptor<StringDescriptor>(getNameId())->convert(format, classFile);

	// no need to convert type
}

