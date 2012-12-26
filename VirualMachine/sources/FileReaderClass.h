#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "IntegerClass.h"
#include "FloatClass.h"


namespace Beer
{
	class VirtualMachine;

	class FileReader : public Object
	{
		//EXTENDING_COLLECTED_OBJECT_ADDING_0();

	protected:
		std::ifstream* mFile;

	public:
		INLINE bool open(const char* name)
		{
			mFile->open(name);
			return mFile->is_open();
		}
		
		INLINE void close()
		{
			mFile->close();
		}

		INLINE void read(Integer::IntegerData& out)
		{
			(*mFile) >> out;
		}

		INLINE void read(Float::FloatData& out)
		{
			(*mFile) >> out;
		}

		INLINE void read(std::string& str)
		{
			(*mFile) >> str;
		}

		INLINE bool failed()
		{
			return mFile->fail();
		}

		INLINE bool atEnd()
		{
			return mFile->eof();
		}

		friend class FileReaderClass;
	};

	class FileReaderClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(StackFrame* frame, GarbageCollector* gc)
		{
			// TODO: constructor
			FileReader* reader = gc->alloc<FileReader>();
			reader->setClass(this);
			reader->mFile = new std::ifstream(); // memory leak!
			return reader;
		}
	};

	class FileReaderClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, std::string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};
