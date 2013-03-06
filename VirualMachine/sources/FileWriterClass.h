#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "Thread.h"
#include "Heap.h"
#include "ClassLoader.h"
#include "IntegerClass.h"
#include "FloatClass.h"
#include "BooleanClass.h"
#include "CharacterClass.h"


namespace Beer
{
	class VirtualMachine;

	class FileWriter : public Object
	{
	protected:
		std::ofstream* mFile;

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

		INLINE void write(Integer::IntegerData out)
		{
			(*mFile) << out;
		}

		INLINE void write(Float::FloatData out)
		{
			(*mFile) << std::setprecision(8) << std::fixed << out;
		}

		INLINE void write(Boolean::BooleanData out)
		{
			if(out) (*mFile) << "true";
			else (*mFile) << "false";
		}

		/*INLINE void write(Character::CharacterData out)
		{
			(*mFile) << out;
		}*/

		INLINE void write(const char_t* str)
		{
			(*mFile) << str;
		}

		INLINE bool failed()
		{
			return mFile->fail();
		}

		friend class FileWriterClass;
	};

	class FileWriterClass : public Class
	{
	public:
		// ClassReflection
		Object* createInstance(Thread* thread, StackFrame* frame) // TODO!!!
		{
			// TODO: constructor
			
			FileWriter* writer = thread->getHeap()->alloc<FileWriter>(Object::OBJECT_CHILDREN_COUNT);
			writer->setClass(this);
			writer->mFile = new std::ofstream(); // memory leak!
			return writer;
		}
	};

	class FileWriterClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(VirtualMachine* vm, ClassLoader* loader, String* name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, Class* klass);
	};
};
