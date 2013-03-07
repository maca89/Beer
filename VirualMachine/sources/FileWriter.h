#pragma once
#include "prereq.h"
#include "Object.h"
#include "Thread.h"
#include "Heap.h"
#include "ClassLoader.h"
#include "Boolean.h"
#include "Character.h"
#include "Class.h"


namespace Beer
{
	class VirtualMachine;
	class Integer;
	class Float;
	class Boolean;
	class Array;
	class String;

	class FileWriter : public Object
	{
	protected:
		std::ofstream* mFile;

	public:
		static void BEER_CALL init(Thread* thread, StackRef<FileWriter> receiver, StackRef<FileWriter> ret);
		static void BEER_CALL open(Thread* thread, StackRef<FileWriter> receiver, StackRef<String> filename, StackRef<Boolean> ret);
		static void BEER_CALL close(Thread* thread, StackRef<FileWriter> receiver);
		static void BEER_CALL writeInteger(Thread* thread, StackRef<FileWriter> receiver, StackRef<Integer> arg);
		static void BEER_CALL writeFloat(Thread* thread, StackRef<FileWriter> receiver, StackRef<Float> arg);
		static void BEER_CALL writeString(Thread* thread, StackRef<FileWriter> receiver, StackRef<String> arg);
		static void BEER_CALL writeBoolean(Thread* thread, StackRef<FileWriter> receiver, StackRef<Boolean> arg);
		static void BEER_CALL writeArray(Thread* thread, StackRef<FileWriter> receiver, StackRef<Array> arg);
		static void BEER_CALL writeLn(Thread* thread, StackRef<FileWriter> receiver);
		static void BEER_CALL writeFailed(Thread* thread, StackRef<FileWriter> receiver, StackRef<Boolean> ret);

	};

	class FileWriterClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
