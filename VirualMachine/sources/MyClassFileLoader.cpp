#include "stdafx.h"
#include "MyClassFileLoader.h"

#include "Thread.h"
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

//#include "String.h"

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
		throw ClassFileException(BEER_WIDEN("Unknown file format"));
	}

	if(format.need()) classFile->convert(format);

	return classFile;
}

void MyClassFileLoader::printClassFile(ClassFileDescriptor* classFile)
{
	cout << "// v" << classFile->getVersion().getMajor()<< "." << classFile->getVersion().getMinor() << std::endl;
	cout << "// " << classFile->getDescriptorsLength() << " descriptors" << std::endl;
	cout << "// " << classFile->getExternalClassesLength() << " external classes" << std::endl;
	cout << "// " << classFile->getDefinedClassesLength() << " defined classes" << std::endl;
	//cout << std::endl;

	for(uint16 classi = 0; classi < classFile->getDefinedClassesLength(); classi++)
	{
		ClassDescriptor* classDescr = classFile->getDescriptor<ClassDescriptor>(classFile->getClassId(classFile->getExternalClassesLength() + classi));

		cout << std::endl;
		cout << "class " << classDescr->getName(classFile)->c_str() << std::endl << "{" << std::endl;
				
		cout << "\tdata" << std::endl << "\t{" << std::endl;
		for(uint16 i = 0; i < classDescr->getAttributesLength(); i++)
		{
			AttributeDescriptor* attrDescr = classFile->getDescriptor<AttributeDescriptor>(classDescr->getAttributeId(i));
			cout << "\t\t" << classFile->getClassName(attrDescr->getTypeId())->c_str() << " " << attrDescr->getName(classFile)->c_str() << std::endl;
		}
		cout << "\t}" << std::endl;

		cout << "\tinterface" << std::endl << "\t{" << std::endl;
		for(uint16 i = 0; i < classDescr->getMethodsLength(); i++)
		{
			MethodDescriptor* methodDescr = classFile->getDescriptor<MethodDescriptor>(classDescr->getMethodId(i));
			cout << "\t\t";
					
			// returns
			for(uint16 reti = 0; reti < methodDescr->getReturnsLength(); reti++)
			{
				AttributeDescriptor* returnDescr = classFile->getDescriptor<AttributeDescriptor>(methodDescr->getReturnId(reti));
				cout << returnDescr->getName(classFile)->c_str();
				cout << " "; // always
			}

			// name
			cout << methodDescr->getName(classFile)->c_str();

			// params
			if(methodDescr->getParamsLength() > 0) cout << "(";
			for(uint16 parami = 0; parami < methodDescr->getParamsLength(); parami++)
			{
				ParamDescriptor* paramDescr = classFile->getDescriptor<ParamDescriptor>(methodDescr->getParamId(parami));
				
				cout << classFile->getClassName(paramDescr->getTypeId())->c_str();		
				cout << " " << paramDescr->getName(classFile)->c_str();

				if(parami < methodDescr->getParamsLength() - 1) // only between
				{
					cout << ", ";
				}
			}
			if(methodDescr->getParamsLength() > 0) cout << ")";

			//methodDescr->getSelector // TODO

			if(!methodDescr->isAbstract() && !methodDescr->isNative())
			{
				cout << std::endl;
				cout << "\t\t{" << std::endl;
				
				cout << "\t\t\t// maxStack = " << methodDescr->getMaxStack() << std::endl;
				if(methodDescr->isOverride()) cout << "\t\t\t// override " << methodDescr->getInterfaceName(classFile)->c_str() << std::endl;
				if(methodDescr->isStatic()) cout << "\t\t\t// static" << std::endl;
				if(methodDescr->isSpecial()) cout << "\t\t\t// special" << std::endl;
				if(methodDescr->isImmutable()) cout << "\t\t\t// immutable" << std::endl;

				uint16 instri = 0;
				BytecodeDescriptor* bc = classFile->getDescriptor<BytecodeDescriptor>(methodDescr->getBytecodeId());
				for(uint16 bytei = 0; bytei < bc->getSize();)
				{
					const Bytecode::Instruction* instr = reinterpret_cast<const Bytecode::Instruction*>(&bc->getByte(bytei));
					cout << "\t\t\t";
					cout << std::setw(3);
					cout << std::setfill(BEER_WIDEN(' ')) << instri++ << ": ";
					bytei += instr->printRaw(classFile);
					cout << std::endl;
				}
			
				cout << "\t\t}" << std::endl;
			}
			else
			{
				cout << ";" << std::endl;
			}
		}
		cout << "\t}" << std::endl;

		cout << "}" << std::endl;
	}
}

void MyClassFileLoader::loadClass(Thread* thread, ClassFileDescriptor* classFile, ClassDescriptor* classDescr)
{
	ClassLoader* loader = thread->getVM()->getClassLoader();
	const char16* name_narrow = classDescr->getName(classFile)->c_str();
	Reference<String> name = String::gTranslate(thread, name_narrow);

	loader->addClassInitializer(
		name.translate(thread->getGC())->c_str(), 
		new LoadedObjectInitializer(classFile, classDescr)
	);
}
		
void MyClassFileLoader::loadClasses(Thread* thread, ClassFileDescriptor* classFile)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	// TODO: check external classes and load them

	for(uint16 classi = 0; classi < classFile->getDefinedClassesLength(); classi++)
	{
		ClassDescriptor* classDescr = classFile->getDescriptor<ClassDescriptor>(classFile->getClassId(classFile->getExternalClassesLength() + classi));
		const StringDescriptor* classNameDescr = classDescr->getName(classFile);
		loadClass(thread, classFile, classDescr); // lazy load because of parents and interfaces
	}

	for(uint16 classi = 0; classi < classFile->getDefinedClassesLength(); classi++)
	{
		ClassDescriptor* classDescr = classFile->getDescriptor<ClassDescriptor>(classFile->getClassId(classFile->getExternalClassesLength() + classi));
		const StringDescriptor* classNameDescr = classDescr->getName(classFile);
		const char16* cclassName = classNameDescr->c_str();

		StackRef<String> className(frame, frame->stackPush(
			*String::gTranslate(thread, cclassName)
		));

		thread->getVM()->findClass(className); // actual load, TODO

		frame->stackMoveTop(-1); // pop className
	}
}