#pragma once
#include "prereq.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "Selector.h"
#include "StackFrame.h"
//#include "IntegerClass.h"
//#include "FloatClass.h"
#include "StringClass.h"
//#include "BooleanClass.h"
#include "ClassTable.h"


namespace Beer
{
	class ClassReflection;
	class ObjectClass;
	class StringClass;
	class BooleanClass;
	class IntegerClass;
	//class ConsoleClass;

	class Integer;
	class Float;
	class Boolean;
	class String;

	struct GarbageCollector;
	class ClassFileLoader;
	class ClassLoader;

	class Debugger;

	class VirtualMachine
	{
	public:
		typedef std::map<std::string, ClassReflection*> ClassReflectionTable;

		WorkStack* mStack;
		Frames* mFrames;
		GarbageCollector* mHeap;
		//GarbageCollector* mClassHeap;
		ClassReflectionTable mClasses;
		ClassLoader* mClassLoader;
		Debugger* mDebugger;
		ClassTable mClassTable;

		uint32 mIntegerClassId;
		uint32 mFloatClassId;
		uint32 mBooleanClassId;
		uint32 mCharacterClassId;

	public:
		INLINE VirtualMachine()
			: mStack(NULL), mFrames(NULL), mHeap(NULL), mClassLoader(NULL), mDebugger(NULL)//,mClassHeap(NULL)
		{
			mIntegerClassId = 0;
			mFloatClassId = 0;
			mBooleanClassId = 0;
			mCharacterClassId = 0;
		}

		INLINE ~VirtualMachine()
		{
		}

		ClassLoader* getClassLoader() const { return mClassLoader; }
	
		void addClass(ClassReflection* reflection);
		void removeClass(ClassReflection* reflection);
		bool hasClass(std::string name) const;
		ClassReflection* getClass(std::string name);
		
		template <typename T>
		INLINE T* getClass(std::string name)
		{
			ClassReflection* klass = getClass(name);
			if(klass) return static_cast<T*>(klass);
			return NULL;
		}

		INLINE const Frames* getStackFrames() const { return mFrames; }
		INLINE bool hasStackFrame() const { return mFrames->size() > 0; }
		INLINE StackFrame* getStackFrame() const { return &mFrames->back();/*(0);*/ }
		StackFrame* openStackFrame(Object* receiver, const char* selector);
		StackFrame* openStackFrame(MethodReflection* method);
		StackFrame* openStackFrame();
		void closeStackFrame();

		//INLINE GarbageCollector* getClassHeap() const { return mClassHeap; }
		INLINE GarbageCollector* getHeap() const { return mHeap; }
		INLINE WorkStack* getStack() const { return mStack; }
		INLINE Debugger* getDebugger() const { return mDebugger; }
		INLINE ClassTable* getClassTable() { return &mClassTable; }

		void init(uint32 stackInitSize = 1*1024, uint32 heapInitSize = 200*1024*1024);
		void run();
		void destroy();

		void invoke(StackFrame* frame);

		Integer* createInteger(int32/*Integer::IntegerData*/ value);
		Float* createFloat(float64/*Float::FloatData*/ value);
		Boolean* createBoolean(bool/*Boolean::BooleanData*/ value);
		//String* createString(const char* value);
		String* createString(String::LengthData length);
		String* createString(std::string s);

	protected:
	};
};