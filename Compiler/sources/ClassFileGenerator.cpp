#include "stdafx.h"

#include "ClassFileGenerator.h"

#include "Block.h"
#include "BlockNode.h"
#include "ByteCodeGenerator.h"
#include "Program.h"
#include "StringDescriptor.h"

using namespace Beer::CodeGeneration;

ClassFileGenerator::ClassFileGenerator(std::ostream & out)
	:	mOut(out),
		mFormat(BigEndian),
		mBwOut(mOut, mFormat == BigEndian), // x86 uses Little endian
		mBwTemp(mTemp, mFormat == BigEndian), // x86 uses Little endian
		mAddr(0)
{
}

void ClassFileGenerator::generateClassFile(::Beer::SemanticAnalysis::Program * program)
{
	generateHeader(VERSION, ClassFileGenerator::Debug);

	mAddr = getClassFileDescriptorSize(program);

	for (SemanticAnalysis::ClassList::iterator it = program->getExternalClasses()->begin(); it != program->getExternalClasses()->end(); it++)
	{
		generateExternalClassDescriptor(*it);
	}

	for (SemanticAnalysis::ClassList::iterator it = program->getClasses()->begin(); it != program->getClasses()->end(); it++)
	{
		generateClassDescriptor(*it);
	}

	generateDescriptorDictionary(program->getDescriptorDictionary());

	mBwOut.write((count)program->getExternalClasses()->size());
	mBwOut.write((count)program->getClasses()->size());

	for (SemanticAnalysis::ClassList::iterator it = program->getExternalClasses()->begin(); it != program->getExternalClasses()->end(); it++)
	{
		mBwOut.write((*it)->getName()->getDescriptorId());
	}

	for (SemanticAnalysis::ClassList::iterator it = program->getClasses()->begin(); it != program->getClasses()->end(); it++)
	{
		mBwOut.write((*it)->getDescriptorId());
	}

	generateEnd();
}

void ClassFileGenerator::generateHeader(version version, compiler_flags flags)
{
	mBwOut.write(mFormat);
	mBwOut.write(flags);
	mBwOut.write(version);
}

void ClassFileGenerator::generateDescriptorDictionary(::Beer::SemanticAnalysis::DescriptorDictionary * dictionary)
{
	for (SemanticAnalysis::StringDescriptorMap::iterator it = dictionary->getStringDescriptors()->begin(); it != dictionary->getStringDescriptors()->end(); it++)
	{
		generateStringDescriptor(it->second);
	}

	mBwOut.write((length)dictionary->getCount());

	for (SemanticAnalysis::DescriptorList::iterator it = dictionary->getDescriptors()->begin(); it != dictionary->getDescriptors()->end(); it++)
	{		
		ASSERT((*it)->getAddress(), "Descriptor is not generated into class file");

		mAddr += mBwOut.write((*it)->getAddress());
	}
}

void ClassFileGenerator::generateExternalClassDescriptor(::Beer::SemanticAnalysis::Class * cls)
{
	cls->setAddress(mAddr);

	generateStringDescriptor(cls->getName());	

	SemanticAnalysis::MethodList * methods = cls->getMethods();

	for (SemanticAnalysis::MethodList::iterator it = methods->begin(); it != methods->end(); it++)
	{
		if ((*it)->getDeclarator() == cls)
		{
			generateMethodDescriptor(*it);
		}
	}
}

void ClassFileGenerator::generateClassDescriptor(::Beer::SemanticAnalysis::Class * cls)
{
	SemanticAnalysis::MemberList * members = cls->getMembers();
	SemanticAnalysis::MethodList * methods = cls->getMethods();

	cls->setAddress(mAddr);

	mAddr += mBwTemp.write(cls->getFlags());
	mAddr += mBwTemp.write(cls->getName()->getDescriptorId());
	mAddr += mBwTemp.write((count)cls->getParents()->size());

	for (SemanticAnalysis::ClassList::iterator it = cls->getParents()->begin(); it != cls->getParents()->end(); it++)
	{
		mAddr += mBwTemp.write((*it)->getId());
	}

	mAddr += mBwTemp.write((count)members->size());

	for (SemanticAnalysis::MemberList::iterator it = cls->getMembers()->begin(); it != members->end(); it++)
	{
		mAddr += mBwTemp.write((*it)->getDescriptorId());
	}

	mAddr += mBwTemp.write((count)methods->size());

	for (SemanticAnalysis::MethodList::iterator it = methods->begin(); it != methods->end(); it++)
	{
		mAddr += mBwTemp.write((*it)->getDescriptorId());
	}

	generateStringDescriptor(cls->getName());

	for (SemanticAnalysis::MemberList::iterator it = members->begin(); it != members->end(); it++)
	{
		generateMemberDescriptor(*it);
	}

	for (SemanticAnalysis::MethodList::iterator it = methods->begin(); it != methods->end(); it++)
	{
		generateMethodDescriptor(*it);
	}
}

void ClassFileGenerator::generateMemberDescriptor(::Beer::SemanticAnalysis::Member * member)
{
	member->setAddress(mAddr);

	mAddr += mBwTemp.write(member->getFlags());
	mAddr += mBwTemp.write(member->getName()->getDescriptorId());
	mAddr += mBwTemp.write(member->getType()->getId());

	generateStringDescriptor(member->getName());
}

void ClassFileGenerator::generateMethodParameterDescriptor(::Beer::SemanticAnalysis::MethodParameter * parameter)
{
	parameter->setAddress(mAddr);

	mAddr += mBwTemp.write(parameter->getFlags());
	mAddr += mBwTemp.write(parameter->getName()->getDescriptorId());
	mAddr += mBwTemp.write(parameter->getType()->getId());

	generateStringDescriptor(parameter->getName());
}

void ClassFileGenerator::generateMethodDescriptor(::Beer::SemanticAnalysis::Method * method)
{
	SemanticAnalysis::MethodParameterList * parameters = method->getParameters();
	SemanticAnalysis::MethodParameterList * returnParameters = method->getReturnParameters();

	method->setAddress(mAddr);

	mAddr += mBwTemp.write(method->getFlags());
	mAddr += mBwTemp.write(method->getName()->getDescriptorId());
	if (method->getDeclarator() != method->getImplementor()) 
	{
		mAddr += mBwTemp.write(method->getDeclarator()->getId());
	}
	else
	{
		mAddr += mBwTemp.write((count)0);
	}

	mAddr += mBwTemp.write((int16)0);

	mAddr += mBwTemp.write((count)parameters->size());

	for (SemanticAnalysis::MethodParameterList::iterator it = parameters->begin(); it != parameters->end(); it++)
	{
		mAddr += mBwTemp.write((*it)->getDescriptorId());
	}

	mAddr += mBwTemp.write((count)returnParameters->size());

	for (SemanticAnalysis::MethodParameterList::iterator it = returnParameters->begin(); it != returnParameters->end(); it++)
	{
		mAddr += mBwTemp.write((*it)->getDescriptorId());
	}

	if (method->isImplemented())
	{
		mAddr += mBwTemp.write(method->getBlock()->getDescriptorId());
	}

	generateStringDescriptor(method->getName());
	generateStringDescriptor(method->getFullName());

	for (SemanticAnalysis::MethodParameterList::iterator it = parameters->begin(); it != parameters->end(); it++)
	{
		generateMethodParameterDescriptor(*it);
	}

	for (SemanticAnalysis::MethodParameterList::iterator it = returnParameters->begin(); it != returnParameters->end(); it++)
	{
		generateMethodParameterDescriptor(*it);
	}

	if (method->isImplemented())
	{	
		generateByteCode(method);
	}
}

void ClassFileGenerator::generateStringDescriptor(::Beer::SemanticAnalysis::StringDescriptor * string)
{
	if (string->getAddress() == 0)
	{
		string->setAddress(mAddr);

		mAddr += mBwTemp.write((int8)0);
		mAddr += mBwTemp.write((length)((string->getValue().length() + 1) * 2));
		mAddr += mBwTemp.write(string->getValue());
	}
}

void ClassFileGenerator::generateByteCode(::Beer::SemanticAnalysis::Method * method)
{
	method->getBlock()->setAddress(mAddr);

	std::stringstream out(std::ios::out | std::ios::binary);

	ByteCodeGenerator bcg(out, method, method->getBlock(), mFormat == BigEndian);

	method->getBlock()->getBlock()->accept(&bcg);

	mAddr += mBwTemp.write((byte_code_flags)0);
	mAddr += mBwTemp.write(bcg.getInstructionCount());
	mAddr += mBwTemp.write(bcg.getInstructionLength());

	mAddr += mBwTemp.write(out);
}

void ClassFileGenerator::generateEnd()
{
	mBwOut.write(mTemp);
}

class_file_address ClassFileGenerator::getClassFileDescriptorSize(::Beer::SemanticAnalysis::Program * program) const
{
	class_file_address address = sizeof(format) + sizeof(compiler_flags) + sizeof(version);

	address += sizeof(count) + (program->getDescriptorDictionary()->getCount() * sizeof(SemanticAnalysis::Descriptor::descriptor_id));
	address += sizeof(count) + program->getExternalClasses()->size() * (sizeof(class_file_address));
	address += sizeof(count) + program->getClasses()->size() * (sizeof(class_file_address));

	return address;
}