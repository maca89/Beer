#pragma once
#include "prereq.h"
#include "ObjectClass.h"
#include "GarbageCollector.h"
#include "ClassLoader.h"
#include "CachedOutput.h"
#include "IntegerClass.h"


namespace Beer
{
	class VirtualMachine;
	class ConsoleClass;

	class Console : public Object
	{
		struct ConsoleArguments
		{

			Integer::IntegerData length;
			const char_t** args;

			ConsoleArguments(Integer::IntegerData length = 0, const char_t** args = NULL) : length(length), args(args) {}
		};

	protected:
		bool mReadFailed;

		static CachedOutput gOut;
		static ConsoleArguments gArguments;

	public:
		INLINE void setReadFailed(bool value) { mReadFailed = value; }
		INLINE bool getReadFailed() const { return mReadFailed; }

		INLINE static CachedOutput& getOutput() { return gOut; }

		INLINE static bool hasArg(Integer::IntegerData i) { return i < gArguments.length; }
		INLINE static const char_t* getArg(Integer::IntegerData i) { return gArguments.args[i]; }
		INLINE static void setArgs(const char_t** args, Integer::IntegerData length) { gArguments.args = args; gArguments.length = length; }

		friend class ConsoleClass;
	};

	class ConsoleClass : public ClassReflection
	{
	public:
		// ClassReflection
		virtual Object* createInstance(VirtualMachine* vm, StackFrame* frame, GarbageCollector* gc)
		{
			Console* cons = gc->alloc<Console>();
			cons->setClass(this);
			cons->setReadFailed(false);
			return cons;
		}

		virtual Object* cloneShallow(VirtualMachine* vm, Object* object, StackFrame* frame, GarbageCollector* gc)
		{
			Console* cons = static_cast<Console*>(this->ClassReflection::cloneShallow(vm, object, frame, gc));
			cons->setReadFailed(object->getInstance<Console>()->getReadFailed());
			return cons;
		}
	};

	class ConsoleClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual ClassReflection* createClass(VirtualMachine* vm, ClassLoader* loader, string name);
		virtual void initClass(VirtualMachine* vm, ClassLoader* loader, ClassReflection* klass);
	};
};
