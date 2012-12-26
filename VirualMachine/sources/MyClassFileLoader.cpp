#include "stdafx.h"
#include "MyClassFileLoader.h"

#include "VirtualMachine.h"
#include "ClassLoader.h"
#include "LoadedObject.h"
#include "Bytecode.h"

#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"
#include "ClassDescriptor.h"
#include "MethodDescriptor.h"
#include "AttributeDescriptor.h"
#include "ParamDescriptor.h"
#include "StringDescriptor.h"
#include "BytecodeDescriptor.h"

using namespace Beer;


ClassFileDescriptor* MyClassFileLoader::loadClassFile(byte* data, uint32 dataLength)
{
	FileFormatConverter format;
	ClassFileDescriptor* classFile = reinterpret_cast<ClassFileDescriptor*>(data);
			
	uint8 fileFormat = classFile->getFileFormat();

	if(fileFormat == FORMAT_BIG_ENDIAN)
	{
		format.setSource(FORMAT_BIG_ENDIAN);
	}
	else if(fileFormat == FORMAT_LITTLE_ENDIAN)
	{
		format.setSource(FORMAT_LITTLE_ENDIAN);
	}
	else
	{
		throw ClassFileException("Unknown file format");
	}

	classFile->convert(format);

	return classFile;
}

void MyClassFileLoader::printClassFile(ClassFileDescriptor* classFile)
{
	std::cout << "// v" << classFile->getVersion().getMajor()<< "." << classFile->getVersion().getMinor() << std::endl;
	std::cout << "// " << classFile->getDescriptorsLength() << " descriptors" << std::endl;
	std::cout << "// " << classFile->getExternalClassesLength() << " external classes" << std::endl;
	std::cout << "// " << classFile->getDefinedClassesLength() << " defined classes" << std::endl;
	//std::cout << std::endl;

	for(uint16 classi = 0; classi < classFile->getDefinedClassesLength(); classi++)
	{
		ClassDescriptor* classDescr = classFile->getDescriptor<ClassDescriptor>(classFile->getClassId(classFile->getExternalClassesLength() + classi));

		std::cout << std::endl;
		std::cout << "class " << classDescr->getName(classFile)->c_str() << std::endl << "{" << std::endl;
				
		std::cout << "\tdata" << std::endl << "\t{" << std::endl;
		for(uint16 i = 0; i < classDescr->getAttributesLength(); i++)
		{
			AttributeDescriptor* attrDescr = classFile->getDescriptor<AttributeDescriptor>(classDescr->getAttributeId(i));
			std::cout << "\t\t" << classFile->getClassName(attrDescr->getTypeId())->c_str() << " " << attrDescr->getName(classFile)->c_str() << std::endl;
		}
		std::cout << "\t}" << std::endl;

		std::cout << "\tinterface" << std::endl << "\t{" << std::endl;
		for(uint16 i = 0; i < classDescr->getMethodsLength(); i++)
		{
			MethodDescriptor* methodDescr = classFile->getDescriptor<MethodDescriptor>(classDescr->getMethodId(i));
			std::cout << "\t\t";
					
			// returns
			for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
			{
				AttributeDescriptor* returnDescr = classFile->getDescriptor<AttributeDescriptor>(methodDescr->getReturnId(reti));
				std::cout << returnDescr->getName(classFile)->c_str();
				std::cout << " "; // always
			}

			// name
			std::cout << methodDescr->getName(classFile)->c_str();

			// params
			if(methodDescr->getParamsLength() > 0) std::cout << " (";
			for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
			{
				ParamDescriptor* paramDescr = classFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
				
				std::cout << classFile->getClassName(paramDescr->getTypeId())->c_str();		
				std::cout << " " << paramDescr->getName(classFile)->c_str();

				if(parami < methodDescr->getParamsLength() - 1) // only between
				{
					std::cout << ", ";
				}
			}
			if(methodDescr->getParamsLength() > 0) std::cout << ")";

			//methodDescr->getSelector // TODO

			std::cout << std::endl;

			std::cout << "\t\t{" << std::endl;
			
			uint16 instri = 0;
			BytecodeDescriptor* bc = classFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
			for(uint16 bytei = 0; bytei < bc->getSize();)
			{
				const Bytecode::Instruction* instr = reinterpret_cast<const Bytecode::Instruction*>(&bc->getByte(bytei));
				std::cout << "\t\t\t";
				std::cout << std::setw(3) << std::setfill(' ') << instri++ << ": ";
				bytei += instr->printRaw(classFile);
				std::cout << std::endl;
			}
			
			std::cout << "\t\t}" << std::endl;
		}
		std::cout << "\t}" << std::endl;

		std::cout << "}" << std::endl;
	}
}

void MyClassFileLoader::loadClass(VirtualMachine* vm, ClassFileDescriptor* classFile, ClassDescriptor* classDescr)
{
	ClassLoader* loader = vm->getClassLoader();
			
	loader->addClassInitializer(
		classDescr->getName(classFile)->c_str(), 
		new LoadedObjectInitializer(classFile, classDescr)
	);
}
		
void MyClassFileLoader::loadClasses(VirtualMachine* vm, ClassFileDescriptor* classFile)
{
	// TODO: check external classes and load them

	for(uint16 classi = 0; classi < classFile->getDefinedClassesLength(); classi++)
	{
		ClassDescriptor* classDescr = classFile->getDescriptor<ClassDescriptor>(classFile->getClassId(classFile->getExternalClassesLength() + classi));
		loadClass(vm, classFile, classDescr); // lazy load because of parents and interfaces
	}

	for(uint16 classi = 0; classi < classFile->getDefinedClassesLength(); classi++)
	{
		ClassDescriptor* classDescr = classFile->getDescriptor<ClassDescriptor>(classFile->getClassId(classFile->getExternalClassesLength() + classi));
		vm->getClass(classDescr->getName(classFile)->c_str()); // actual load
	}
}