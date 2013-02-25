#pragma once
#include "prereq.h"
#include "Bytecode.h"
#include "StackFrame.h"


namespace Beer
{
	class VirtualMachine;
	class Object;
	class StackFrame;
	class Class;

	class Debugger
	{
	public:
		typedef std::list<Object*> ObjectList;

	protected:
		VirtualMachine* mVM;
		bool mEnabled;
		bool mStepping;
		ObjectList mPrintedObjects;

	public:
		Debugger(VirtualMachine* vm);
		virtual ~Debugger();

		virtual void setEnabled(bool value) { mEnabled = value; }
		INLINE bool isEnabled() const { return mEnabled; }
		
		virtual void started();
		virtual void step(StackFrame* frame);
		virtual void ended();

		virtual void setSteppingMode(bool value) { mStepping = value; }
		INLINE bool isStepping() const { return mStepping && mEnabled; }

		virtual bool catchException(StackFrame* frame, const Exception& ex);

		virtual void printLastOutput();
		virtual void printNativeInstruction();
		virtual void printInstruction(const Bytecode::Instruction* instr, uint16 programCounter);
		virtual void printObject(Object* object);
		virtual void printCallStack(StackFrame* frame);
		virtual void printFrame(StackFrame* frame);
		virtual void printFrameStack(StackFrame* frame);
		virtual void printStack();
		virtual void printClassMethods(Class* klass);
	};
}
