#pragma once
#include "prereq.h"
#include "Object.h"
#include "GenerationalGC.h"
#include "ClassLoader.h"
#include "CachedOutput.h"
#include "ThreadSafeOutput.h"
#include "Integer.h"


namespace Beer
{
	class VirtualMachine;
	class ConsoleClass;
	class Array;
	class Float;

	class Console : public Object
	{
	public:
		typedef CachedOutput Output;

		struct ConsoleArguments
		{

			Integer::IntegerData length;
			const char_t** args;

			INLINE ConsoleArguments(Integer::IntegerData length = 0, const char_t** args = NULL) : length(length), args(args) {}
		};

	protected:
		bool mReadFailed;

		static Output gOut;
		static ConsoleArguments gArguments;

	public:
		INLINE void setReadFailed(bool value) { mReadFailed = value; }
		INLINE bool getReadFailed() const { return mReadFailed; }

		INLINE static ThreadSafeOutput getOutput() { return ThreadSafeOutput(cout); }

		INLINE static bool gHasArg(Integer::IntegerData i) { return i < gArguments.length; }
		INLINE static const char_t* gGetArg(Integer::IntegerData i) { return gArguments.args[i]; }
		INLINE static void gSetArgs(const char_t** args, Integer::IntegerData length) { gArguments.args = args; gArguments.length = length; }

		static void BEER_CALL init(Thread* thread, StackRef<Console> receiver, StackRef<Console> ret);

		static void BEER_CALL println(Thread* thread, StackRef<Console> receiver);
		
		static void BEER_CALL printInteger(Thread* thread, StackRef<Console> receiver, StackRef<Integer> arg);
		static void BEER_CALL printlnInteger(Thread* thread, StackRef<Console> receiver, StackRef<Integer> arg);
		static void BEER_CALL printFloat(Thread* thread, StackRef<Console> receiver, StackRef<Float> arg);
		static void BEER_CALL printlnFloat(Thread* thread, StackRef<Console> receiver, StackRef<Float> arg);
		static void BEER_CALL printString(Thread* thread, StackRef<Console> receiver, StackRef<String> arg);
		static void BEER_CALL printlnString(Thread* thread, StackRef<Console> receiver, StackRef<String> arg);
		static void BEER_CALL printBoolean(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> arg);
		static void BEER_CALL printlnBoolean(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> arg);
		static void BEER_CALL printArray(Thread* thread, StackRef<Console> receiver, StackRef<Array> arg);
		static void BEER_CALL printlnArray(Thread* thread, StackRef<Console> receiver, StackRef<Array> arg);

		static void BEER_CALL readInteger(Thread* thread, StackRef<Console> receiver, StackRef<Integer> ret);
		static void BEER_CALL readFloat(Thread* thread, StackRef<Console> receiver, StackRef<Float> ret);
		static void BEER_CALL readBoolean(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> ret);
		static void BEER_CALL readLine(Thread* thread, StackRef<Console> receiver, StackRef<String> ret);
		static void BEER_CALL readFailed(Thread* thread, StackRef<Console> receiver, StackRef<Boolean> ret);
		static void BEER_CALL getArg(Thread* thread, StackRef<Console> receiver, StackRef<Integer> index, StackRef<String> ret);
	};

	class ConsoleClassInitializer : public ClassInitializer
	{
	public:
		// ClassInitializer
		virtual Class* createClass(Thread* thread, ClassLoader* loader, String* name);
		virtual void initClass(Thread* thread, ClassLoader* loader, Class* klass);
	};
};
