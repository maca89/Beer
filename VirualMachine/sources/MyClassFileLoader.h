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

		virtual void loadClass(Thread* thread, ClassFileDescriptor* classFile, ClassDescriptor* classDescr);
		virtual void loadClasses(Thread* thread, ClassFileDescriptor* classFile);
	};
};