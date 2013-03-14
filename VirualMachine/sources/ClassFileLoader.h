#pragma once
#include "prereq.h"


namespace Beer
{
	class Thread;
	class ClassFileDescriptor;
	class ClassDescriptor;

	class ClassFileLoader
	{
	public:
		virtual ~ClassFileLoader() {}

		virtual ClassFileDescriptor* loadClassFile(byte* data, uint32 dataLength) = 0;
		virtual void printClassFile(ClassFileDescriptor* classFile) = 0;

		virtual void loadClasses(Thread* thread, ClassFileDescriptor* classFile) = 0;
		virtual void loadClass(Thread* thread, ClassFileDescriptor* classFile, ClassDescriptor* classDescr) = 0;
	};
};