#include "stdafx.h"

BeerlangCompiler::CodeGeneration::ClassFileGenerator::ClassFileGenerator(std::ostream & out)
	: mOut(out), mFormat(BigEndian), mBwOut(mOut, mFormat == BigEndian), mBwTemp(mTemp, mFormat == BigEndian), mCurrentAddress(0) // x86 uses Little endian
{
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateClassFile(SemanticAnalysis::ProgramInfo * program)
{
	generateHeader(VERSION, ClassFileGenerator::Debug);

	mCurrentAddress = getClassFileDescriptorSize(program);

	for (SemanticAnalysis::ClassListIterator it = program->getExternalClasses()->begin(); it != program->getExternalClasses()->end(); it++)
	{
		generateStringDescriptor((*it)->getName());
	}

	for (SemanticAnalysis::ClassListIterator it = program->getClasses()->begin(); it != program->getClasses()->end(); it++)
	{
		generateClassDescriptor(*it);
	}
	
	generateDescriptorDictionary(program->getDescriptorDictionary());

	mBwOut.write((count)program->getExternalClasses()->size());
	mBwOut.write((count)program->getClasses()->size());

	for (SemanticAnalysis::ClassListIterator it = program->getExternalClasses()->begin(); it != program->getExternalClasses()->end(); it++)
	{
		mBwOut.write((*it)->getName()->getDescriptorId());
	}
	
	for (SemanticAnalysis::ClassListIterator it = program->getClasses()->begin(); it != program->getClasses()->end(); it++)
	{
		mBwOut.write((*it)->getDescriptorId());
	}

	generateEnd();
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateHeader(version version, compiler_flags flags)
{
	mBwOut.write(mFormat);
	mBwOut.write(flags);
	mBwOut.write(version);
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateDescriptorDictionary(SemanticAnalysis::DescriptorDictionary * dictionary)
{
	for (SemanticAnalysis::StringConstantMapIterator it = dictionary->getStringDescriptors()->begin(); it != dictionary->getStringDescriptors()->end(); it++)
	{
		generateStringDescriptor(it->second);
	}

	mBwOut.write((length)dictionary->getCount());

	for (SemanticAnalysis::DescriptorListIterator it = dictionary->getDescriptors()->begin(); it != dictionary->getDescriptors()->end(); it++)
	{		
		ASSERT((*it)->getAddress(), "Descriptor is not generated into class file");
		
		mCurrentAddress += mBwOut.write((*it)->getAddress());
	}
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateExternalClassDescriptor(SemanticAnalysis::ClassInfo * cls)
{
	cls->setAddress(mCurrentAddress);

	mCurrentAddress += mBwTemp.write((length)(cls->getName()->getValue().length() + 1));
	mCurrentAddress += mBwTemp.write(cls->getName()->getValue());
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateClassDescriptor(SemanticAnalysis::ClassInfo * cls)
{
	SemanticAnalysis::MemberList * members = cls->getMembers();
	SemanticAnalysis::MethodList * methods = cls->getMethods();

	cls->setAddress(mCurrentAddress);

	mCurrentAddress += mBwTemp.write(cls->getFlags());
	mCurrentAddress += mBwTemp.write(cls->getName()->getDescriptorId());
	mCurrentAddress += mBwTemp.write((count)0);

	mCurrentAddress += mBwTemp.write((count)members->size());
	
	for (SemanticAnalysis::MemberListIterator it = cls->getMembers()->begin(); it != members->end(); it++)
	{
		mCurrentAddress += mBwTemp.write((*it)->getDescriptorId());
	}

	mCurrentAddress += mBwTemp.write((count)methods->size());

	for (SemanticAnalysis::MethodListIterator it = methods->begin(); it != methods->end(); it++)
	{
		mCurrentAddress += mBwTemp.write((*it)->getDescriptorId());
	}

	generateStringDescriptor(cls->getName());

	for (SemanticAnalysis::MemberListIterator it = members->begin(); it != members->end(); it++)
	{
		generateMemberDescriptor(*it);
	}

	for (SemanticAnalysis::MethodListIterator it = methods->begin(); it != methods->end(); it++)
	{
		generateMethodDescriptor(*it);
	}
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateMemberDescriptor(SemanticAnalysis::MemberInfo * member)
{
	member->setAddress(mCurrentAddress);

	mCurrentAddress += mBwTemp.write(member->getFlags());
	mCurrentAddress += mBwTemp.write(member->getName()->getDescriptorId());
	mCurrentAddress += mBwTemp.write(member->getType()->getId());

	generateStringDescriptor(member->getName());
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateMethodParameterDescriptor(SemanticAnalysis::MethodParameterInfo * parameter)
{
	parameter->setAddress(mCurrentAddress);

	mCurrentAddress += mBwTemp.write(parameter->getFlags());
	mCurrentAddress += mBwTemp.write(parameter->getName()->getDescriptorId());
	mCurrentAddress += mBwTemp.write(parameter->getType()->getId());

	generateStringDescriptor(parameter->getName());
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateMethodDescriptor(SemanticAnalysis::MethodInfo * method)
{
	SemanticAnalysis::MethodParameterList * parameters = method->getParameters();
	SemanticAnalysis::MethodParameterList * returnParameters = method->getReturnParameters();
	
	method->setAddress(mCurrentAddress);

	mCurrentAddress += mBwTemp.write(method->getFlags());
	mCurrentAddress += mBwTemp.write(method->getName()->getDescriptorId());

	mCurrentAddress += mBwTemp.write((count)parameters->size());
	
	for (SemanticAnalysis::MethodParameterListIterator it = parameters->begin(); it != parameters->end(); it++)
	{
		mCurrentAddress += mBwTemp.write((*it)->getDescriptorId());
	}

	mCurrentAddress += mBwTemp.write((count)returnParameters->size());

	for (SemanticAnalysis::MethodParameterListIterator it = returnParameters->begin(); it != returnParameters->end(); it++)
	{
		mCurrentAddress += mBwTemp.write((*it)->getDescriptorId());
	}

	mCurrentAddress += mBwTemp.write(method->getBlock()->getDescriptorId());

	generateStringDescriptor(method->getName());
	generateStringDescriptor(method->getFullName());

	for (SemanticAnalysis::MethodParameterListIterator it = parameters->begin(); it != parameters->end(); it++)
	{
		generateMethodParameterDescriptor(*it);
	}

	for (SemanticAnalysis::MethodParameterListIterator it = returnParameters->begin(); it != returnParameters->end(); it++)
	{
		generateMethodParameterDescriptor(*it);
	}

	generateByteCode(method);
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateStringDescriptor(SemanticAnalysis::StringConstant * string)
{
	if (string->getAddress() == 0)
	{
		string->setAddress(mCurrentAddress);

		mCurrentAddress += mBwTemp.write((int8)0);
		mCurrentAddress += mBwTemp.write((length)(string->getValue().length() + 1));
		mCurrentAddress += mBwTemp.write(string->getValue());
	}
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateByteCode(SemanticAnalysis::MethodInfo * method)
{
	method->getBlock()->setAddress(mCurrentAddress);

	std::stringstream out(std::ios::out | std::ios::binary);

	ByteCodeGenerator bcg(out, method, method->getBlock(), mFormat == BigEndian);
	
	method->getBlock()->getBlock()->accept(&bcg);

	mCurrentAddress += mBwTemp.write((byte_code_flags)0);
	mCurrentAddress += mBwTemp.write(bcg.getInstructionCount());
	mCurrentAddress += mBwTemp.write(bcg.getInstructionLength());

	mCurrentAddress += mBwTemp.write(out);
}

void BeerlangCompiler::CodeGeneration::ClassFileGenerator::generateEnd()
{
	mBwOut.write(mTemp);
}

BeerlangCompiler::CodeGeneration::class_file_address BeerlangCompiler::CodeGeneration::ClassFileGenerator::getClassFileDescriptorSize(SemanticAnalysis::ProgramInfo * program) const
{
	class_file_address address = sizeof(format) + sizeof(compiler_flags) + sizeof(version);

	address += sizeof(count) + (program->getDescriptorDictionary()->getCount() * sizeof(SemanticAnalysis::descriptor_id));
	address += sizeof(count) + program->getExternalClasses()->size() * (sizeof(class_file_address));
	address += sizeof(count) + program->getClasses()->size() * (sizeof(class_file_address));

	return address;
}