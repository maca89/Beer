#pragma once
#include "prereq.h"
#include "ClassFileLoader.h"


namespace Beer
{
	class MyClassFileLoader : public ClassFileLoader
	{
	protected:
		

	public:
		// ClassFileLoader

		virtual ClassFileDescriptor* loadClassFile(byte* data, uint32 dataLength);
		virtual void printClassFile(ClassFileDescriptor* classFile);

		virtual void loadClass(VirtualMachine* vm, ClassFileDescriptor* classFile, ClassDescriptor* classDescr);
		virtual void loadClasses(VirtualMachine* vm, ClassFileDescriptor* classFile);
	};
};