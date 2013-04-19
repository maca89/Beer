#pragma once
#include "prereq.h"
#include "Bytecode.h"
#include "Frame.h"
#include "Thread.h"


namespace Beer
{
	class VirtualMachine;
	class Object;
	class Frame;
	class Class;

	class Debugger : protected Thread
	{
	public:
		typedef std::list<Object*> ObjectList;

	protected:
		bool mEnabled;
		bool mStepping;
		ObjectList mPrintedObjects;
		
		// TODO: get rid of this
		TaskContext mTemporaryContext;

	public:
		Debugger(VirtualMachine* vm, GenerationalGC* gc);
		virtual ~Debugger();

		void setEnabled(bool value) { mEnabled = value; }
		INLINE bool isEnabled() const { return mEnabled; }
		
		void init();
		void started();
		void step(Thread* thread, Frame* frame);
		void ended();

		void setSteppingMode(bool value) { mStepping = value; }
		INLINE bool isStepping() const { return mStepping && mEnabled; }

		bool catchException(Thread* thread, Frame* frame, const Exception& ex);

		void printLastOutput();
		void printNativeInstruction();
		void printInstruction(StackRef<Method> method, Bytecode* bc, Bytecode::Instruction* instr, uint16 programCounter);
		void printObject(StackRef<Object> object);
		void printCallStack(Thread* thread, Frame* frame);
		void printFrame(Thread* thread, Frame* frame);
		void printFrameStack(Frame* frame);
		void printClassName(StackRef<Class> klass);
		void printObjectClassName(StackRef<Object> object);
		void printMethodSignature(StackRef<Method> method);
		void printCalledMethodSignature(Frame* frame, StackRef<Object> receiver, StackRef<Method> method);
		void printClassMethods(StackRef<Class> klass);
		void printBytecodeMethods(Class* klass);
		void printBytecodeMethod(StackRef<Class> klass, StackRef<Method> method);
		void printBytecode(StackRef<Method> method, Bytecode* bc);

	protected:
		virtual void work() {}
	};
}
