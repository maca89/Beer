#pragma once
#include "prereq.h"


namespace Beer
{
	class VirtualMachine;
	class ClassFileDescriptor;
	class ClassDescriptor;

	class ClassFileLoader
	{
	public:
		virtual ~ClassFileLoader() {}

		virtual ClassFileDescriptor* loadClassFile(byte* data, uint32 dataLength) = 0;
		virtual void printClassFile(ClassFileDescriptor* classFile) = 0;

		virtual void loadClasses(VirtualMachine* vm, ClassFileDescriptor* classFile) = 0;
		virtual void loadClass(VirtualMachine* vm, ClassFileDescriptor* classFile, ClassDescriptor* classDescr) = 0;
	};
};