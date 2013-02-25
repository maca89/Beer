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
	class Class;
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
	typedef std::map<string, Class*> ClassReflectionTable;

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

		Class* mObjectClass;
		Class* mStringClass;
		Class* mFloatClass;
		Class* mIntegerClass;
		Class* mBooleanClass;
		Class* mCharacterClass;
		//Class* mTaskClass;
		Class* mPairClass;

	public:
		INLINE VirtualMachine()
			: mStack(NULL), /*mFrames(NULL),*/ mHeap(NULL), mClassHeap(NULL),
			mClassLoader(NULL), mDebugger(NULL), 
			mObjectClass(NULL), mStringClass(NULL), mCharacterClass(NULL), mIntegerClass(NULL), mBooleanClass(NULL), mPairClass(NULL)//, mTaskClass(NULL)
		{
		}

		INLINE ~VirtualMachine()
		{
		}
	
		void addClass(Class* reflection);
		//void removeClass(ClassReflection* reflection);
		//bool hasClass(string name) const;
		Class* getClass(String* name);
		
		/*template <typename T>
		INLINE T* getClass(string name)
		{
			ClassReflection* klass = getClass(name);
			if(klass) return static_cast<T*>(klass);
			return NULL;
		}*/

		INLINE Class* getClass(string name) { return getClass(createString(name)); } // TODO: get rid of
		INLINE Class* getClass(Reference<String>& name) { return getClass(*name); }

		INLINE Class* getClass(const StackRef<Object>& object) const { return mClassTable.translate(object); }
		INLINE Class* getClass(Object* object) const { return mClassTable.translate(object); }

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

		// TODO: get rid of these
		Integer* createInteger(Integer::IntegerData value);
		Float* createFloat(Float::FloatData value);
		String* createString(const Character::CharacterData* value);
		String* createString(String::LengthData length);
		String* createString(const string& s);
		Pair* createPair(Object* first, Object* second);

		INLINE Class* getFloatClass() const { return mFloatClass; }
		INLINE Class* getIntegerClass() const { return mIntegerClass; }
		INLINE Class* getBooleanClass() const { return mBooleanClass; }
		INLINE Class* getObjectClass() const { return mObjectClass; }
		INLINE Class* getStringClass() /*const*/ { return mStringClass; }
		INLINE Class* getCharacterClass() const { return mCharacterClass; }
		//INLINE Class* getTaskClass() const { return mTaskClass; }
		INLINE Class* getPairClass() /*const*/ { return getClass(BEER_WIDEN("Pair")); /*return mPairClass;*/ }

		template <typename T>
		INLINE T* getStringClass() const { return static_cast<T*>(mStringClass); }

	protected:
	};
};