#pragma once
#include "prereq.h"
#include "Bytecode.h"
#include "StackFrame.h"
#include "Thread.h"


namespace Beer
{
	class VirtualMachine;
	class Object;
	class StackFrame;
	class Class;

	class Debugger : public Thread
	{
	public:
		typedef std::list<Object*> ObjectList;

	protected:
		bool mEnabled;
		bool mStepping;
		ObjectList mPrintedObjects;

	public:
		Debugger(VirtualMachine* vm, GC* gc);
		virtual ~Debugger();

		virtual void setEnabled(bool value) { mEnabled = value; }
		INLINE bool isEnabled() const { return mEnabled; }
		
		virtual void started();
		virtual void step(Thread* thread, StackFrame* frame);
		virtual void ended();

		virtual void setSteppingMode(bool value) { mStepping = value; }
		INLINE bool isStepping() const { return mStepping && mEnabled; }

		virtual bool catchException(Thread* thread, StackFrame* frame, const Exception& ex);

		virtual void printLastOutput();
		virtual void printNativeInstruction();
		virtual void printInstruction(const Bytecode::Instruction* instr, uint16 programCounter);
		virtual void printObject(StackRef<Object> object);
		virtual void printCallStack(Thread* thread, StackFrame* frame);
		virtual void printFrame(Thread* thread, StackFrame* frame);
		virtual void printFrameStack(StackFrame* frame);
		virtual void printClassName(StackRef<Class> klass);
		virtual void printObjectClassName(StackRef<Object> object);
		virtual void printMethodSignature(StackRef<Method> method);
		virtual void printCalledMethodSignature(StackFrame* frame, StackRef<Object> receiver, StackRef<Method> method);
		virtual void printClassMethods(Class* klass);

	protected:
		virtual void work() {}
	};
}
