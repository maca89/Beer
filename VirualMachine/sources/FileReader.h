#pragma once
#include "prereq.h"
#include "Object.h"
#include "ClassLoader.h"
#include "Class.h"
#include "Integer.h"
#include "Float.h"


namespace Beer
{
	class VirtualMachine;
	class Integer;
	class Float;
	class Boolean;
	class Array;
	class String;

	class FileReader : public Object
	{
	protected:
		ifstream* mFile;

	public:		
		static void BEER_CALL init(Thread* thread, StackRef<FileReader> receiver, StackRef<FileReader> ret);
		static void BEER_CALL open(Thread* thread, StackRef<FileReader> receiver, StackRef<String> filename, StackRef<Boolean> ret);
		static void BEER_CALL close(Thread* thread, StackRef<FileReader> receiver);
		static void BEER_CALL readInteger(Thread* thread, StackRef<FileReader> receiver, StackRef<Integer> ret);
		static void BEER_CALL readFloat(Thread* thread, StackRef<FileReader> receiver, StackRef<Float> ret);
		static void BEER_CALL readString(Thread* thread, StackRef<FileReader> receiver, StackRef<String> ret);
		static void BEER_CALL readFailed(Thread* thread, StackRef<FileReader> receiver, StackRef<Boolean> ret);
		static void BEER_CALL readEnded(Thread* thread, StackRef<FileReader> receiver, StackRef<Boolean> ret);
	};

	class FileReaderClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};
