#pragma once
#include "prereq.h"
#include "FixedStack.h"
#include "DynamicStack.h"
#include "CopyGC.h"
#include "Selector.h"
#include "StackFrame.h"
//#include "IntegerClass.h"
#include "FloatClass.h"
#include "StringClass.h"
#include "CharacterClass.h"
#include "BooleanClass.h"
#include "ClassTable.h"
#include "InlineFunctionTable.h"


namespace Beer
{
	class ClassReflection;
	class ObjectClass;
	class StringClass;
	class BooleanClass;
	class IntegerClass;
	//class ConsoleClass

	class Integer;
	class Float;
	class Boolean;
	class String;

	struct GarbageCollector;
	class ClassFileLoader;
	class ClassLoader;
	class Thread;

	class Debugger;

	typedef CopyGC Heap;
	typedef std::set<Thread*> ThreadSet;
	typedef std::map<string, ClassReflection*> ClassReflectionTable;

	class VirtualMachine
	{
	protected:
		WorkStack* mStack; // get rid of this

		Heap* mHeap;
		GarbageCollector* mClassHeap;
		ClassReflectionTable mClasses;
		ClassLoader* mClassLoader;
		Debugger* mDebugger;
		ClassTable mClassTable;
		InlineFunctionTable mInlineFnTable;
		ThreadSet mThreads;

		ClassReflection* mObjectClass;
		ClassReflection* mStringClass;
		ClassReflection* mFloatClass;
		ClassReflection* mIntegerClass;
		ClassReflection* mBooleanClass;
		ClassReflection* mCharacterClass;
		ClassReflection* mTaskClass;

	public:
		INLINE VirtualMachine()
			: mStack(NULL), /*mFrames(NULL),*/ mHeap(NULL), mClassHeap(NULL),
			mClassLoader(NULL), mDebugger(NULL), 
			mObjectClass(NULL), mStringClass(NULL), mCharacterClass(NULL), mIntegerClass(NULL), mBooleanClass(NULL)
		{
		}

		INLINE ~VirtualMachine()
		{
		}
	
		void addClass(ClassReflection* reflection);
		void removeClass(ClassReflection* reflection);
		bool hasClass(string name) const;
		ClassReflection* getClass(string name);
		
		template <typename T>
		INLINE T* getClass(string name)
		{
			ClassReflection* klass = getClass(name);
			if(klass) return static_cast<T*>(klass);
			return NULL;
		}

		INLINE ClassReflection* getClass(String* name)
		{
			return getClass(name->c_str());
		}

		INLINE Heap* getHeap() const { return mHeap; }
		INLINE WorkStack* getStack() const { return mStack; }
		INLINE Debugger* getDebugger() const { return mDebugger; }
		INLINE ClassTable* getClassTable() { return &mClassTable; }
		INLINE InlineFunctionTable* getInlineFunctionTable() { return &mInlineFnTable; }
		INLINE ThreadSet& getThreads() { return mThreads; }
		INLINE ClassLoader* getClassLoader() const { return mClassLoader; }
		INLINE ClassReflectionTable& getClasses() { return mClasses; }

		void init(uint32 stackInitSize = 1*1024, uint32 heapInitSize = 2*1024*1024);
		void run();
		void destroy();

		Integer* createInteger(Integer::IntegerData value);
		Float* createFloat(Float::FloatData value);
		String* createString(const Character::CharacterData* value);
		String* createString(String::LengthData length);
		String* createString(const string& s);

		INLINE ClassReflection* getFloatClass() const { return mFloatClass; }
		INLINE ClassReflection* getIntegerClass() const { return mIntegerClass; }
		INLINE ClassReflection* getBooleanClass() const { return mBooleanClass; }
		INLINE ClassReflection* getObjectClass() const { return mObjectClass; }
		INLINE ClassReflection* getStringClass() const { return mStringClass; }
		INLINE ClassReflection* getCharacterClass() const { return mCharacterClass; }
		INLINE ClassReflection* getTaskClass() const { return mTaskClass; }

		template <typename T>
		INLINE T* getStringClass() const { return static_cast<T*>(mStringClass); }

	protected:
	};
};