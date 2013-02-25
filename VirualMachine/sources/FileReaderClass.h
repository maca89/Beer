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
	protected:
		ifstream* mFile;

	public:
		INLINE bool open(const char_t* name)
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

		INLINE void read(string& str)
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

	class FileReaderClass : public Class
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			// TODO: constructor
			FileReader* reader = gc->alloc<FileReader>(Object::OBJECT_CHILDREN_COUNT);
			reader->setClass(this);
			reader->mFile = new ifstream(); // memory leak!
			return reader;
		}
	};

	class FileReaderClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
