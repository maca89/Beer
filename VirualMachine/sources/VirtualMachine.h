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
	//class ConsoleClass;

	class Integer;
	class Float;
	class Boolean;
	class String;

	struct GarbageCollector;
	class ClassFileLoader;
	class ClassLoader;

	class Debugger;

	typedef CopyGC Heap;

	class VirtualMachine
	{
	public:
		typedef std::map<string, ClassReflection*> ClassReflectionTable;

		WorkStack* mStack;
		Frames* mFrames;
		Heap* mHeap;
		GarbageCollector* mClassHeap;
		ClassReflectionTable mClasses;
		ClassLoader* mClassLoader;
		Debugger* mDebugger;
		ClassTable mClassTable;
		InlineFunctionTable mInlineFnTable;

		ClassReflection* mObjectClass;
		ClassReflection* mStringClass;
		ClassReflection* mFloatClass;
		ClassReflection* mIntegerClass;
		ClassReflection* mBooleanClass;
		ClassReflection* mCharacterClass;

	public:
		INLINE VirtualMachine()
			: mStack(NULL), mFrames(NULL), mHeap(NULL), mClassHeap(NULL),
			mClassLoader(NULL), mDebugger(NULL), 
			mObjectClass(NULL), mStringClass(NULL), mCharacterClass(NULL), mIntegerClass(NULL), mBooleanClass(NULL)
		{
		}

		INLINE ~VirtualMachine()
		{
		}

		ClassLoader* getClassLoader() const { return mClassLoader; }
	
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

		INLINE const Frames* getStackFrames() const { return mFrames; }
		INLINE bool hasStackFrame() const { return mFrames->size() > 0; }
		INLINE StackFrame* getStackFrame() const { return &mFrames->back();/*(0);*/ }
		StackFrame* openStackFrame(Object* receiver, const char_t* selector);
		StackFrame* openStackFrame(MethodReflection* method);
		StackFrame* openStackFrame();
		void closeStackFrame();

		//INLINE GarbageCollector* getClassHeap() const { return mClassHeap; }
		INLINE Heap* getHeap() const { return mHeap; }
		INLINE WorkStack* getStack() const { return mStack; }
		INLINE Debugger* getDebugger() const { return mDebugger; }
		INLINE ClassTable* getClassTable() { return &mClassTable; }
		INLINE InlineFunctionTable* getInlineFunctionTable() { return &mInlineFnTable; }

		void init(uint32 stackInitSize = 1*1024, uint32 heapInitSize = 2*1024*1024);
		void run();
		void destroy();
		void invoke(StackFrame* frame);

		Integer* createInteger(int32/*Integer::IntegerData*/ value);
		Float* createFloat(Float::FloatData value);
		INLINE Boolean* createBoolean(Boolean::BooleanData value) { return Boolean::makeInlineValue(value); }
		String* createString(const Character::CharacterData* value);
		String* createString(String::LengthData length);
		String* createString(const string& s);

		INLINE ClassReflection* getFloatClass() const { return mFloatClass; }
		INLINE ClassReflection* getIntegerClass() const { return mIntegerClass; }
		INLINE ClassReflection* getBooleanClass() const { return mBooleanClass; }
		INLINE ClassReflection* getObjectClass() const { return mObjectClass; }
		INLINE ClassReflection* getStringClass() const { return mStringClass; }
		INLINE ClassReflection* getCharacterClass() const { return mCharacterClass; }

		template <typename T>
		INLINE T* getStringClass() const { return static_cast<T*>(mStringClass); }

	protected:
	};
};