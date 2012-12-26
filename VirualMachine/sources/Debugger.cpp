#include "stdafx.h"
#include "Debugger.h"
#include "VirtualMachine.h"
#include "MethodReflection.h"
#include "ConsoleClass.h"
#include "PropertyReflection.h"
#include "ClassReflection.h"

using namespace Beer;


Debugger::Debugger(VirtualMachine* vm) : mVM(vm), mEnabled(false), mStepping(false)
{
}

Debugger::~Debugger()
{
}

void Debugger::printLastOutput()
{
	std::cout << "[LastOutput]" << std::endl;
	Console::getOutput().flush(std::cout);
	std::cout << std::endl;
}

void Debugger::printInstruction(const Bytecode::Instruction* instr, uint16 programCounter)
{
	std::cout /*<< std::endl*/ << "[Instruction]" << std::endl;
	std::cout << std::setw(4) << std::setfill(' ') << "+" << programCounter << " ";
	instr->printTranslated();
	std::cout << std::endl;
}

void Debugger::printNativeInstruction()
{
	std::cout /*<< std::endl*/ << "[Instruction]" << std::endl;
	std::cout << std::setw(4) << std::setfill(' ') << "+" << "NATIVE_CODE ";
	std::cout << std::endl;
}

void Debugger::printCallStack()
{
	std::cout << "[CallStack]" << std::endl;
	
	const Frames* frames = mVM->getStackFrames();
	uint32 i = 0;
	for(Frames::const_iterator it = frames->begin(); it != frames->end(); it++)
	{
		const StackFrame& frame = *it;
		std::cout << std::setw(4) << std::setfill(' ') << "+" << i << " ";
		
		if(frame.method)
		{
			std::cout << frame.method << " " << frame.method->getSelector() << "@" << frame.programCounter;
		}
		else
		{
			std::cout << "no method";
		}
		
		std::cout << std::endl;
		i++;
	}
	std::cout << std::endl;
}

void Debugger::printObject(Object* object)
{
	for(ObjectList::const_iterator it = mPrintedObjects.begin(); it != mPrintedObjects.end(); it++)
	{
		if(*it == object)
		{
			std::cout << "...";
			return;
		}
	}

	mPrintedObjects.push_back(object);

	if(object == NULL) std::cout << "NULL";
	else
	{
		ClassReflection* klass = mVM->getClassTable()->translate(object);
		if(klass)
		{
			if(!klass->isValueType()) std::cout << "#" << object << " ";

			std::stringstream ss;
			klass->dump(object, ss);
			std::cout << ss.str();

			if(klass->getPropertiesCount() > 0)
			{
				std::cout << " {";
				for(uint32 i = 0; i < klass->getPropertiesCount(); i++)
				{
					PropertyReflection* prop = klass->getProperty(i);
					Object* child = object->getChild<Object>(i);
				
					if(prop) std::cout << prop->getName() << ": ";

					if(child == NULL)
					{
						std::cout << "null";
					}
					else
					{
						printObject(child);
					}

					if(i < klass->getPropertiesCount() - 1) std::cout << ", ";
				}
				std::cout << "}";
			}
		}
		else
		{
			std::cout << "#" << object << " has no class";
		}
	}

	mPrintedObjects.pop_back();
}

void Debugger::printFrame(StackFrame* frame)
{
	printCallStack();
	printFrameStack(frame);

	if(frame->method->isBytecode())
	{
		const Bytecode::Instruction* instr = frame->method->getBytecode()->getInstruction(frame->programCounter);
		printInstruction(instr, frame->programCounter);
	}
	else
	{
		printNativeInstruction(/*frame*/);
	}
}

void Debugger::printFrameStack(StackFrame* frame)
{
	uint32 i = 0; // TODO
	WorkStack* stack = mVM->getStack();

	std::cout << "[FrameStack|offset=" << frame->frameOffset << "]" << std::endl;

	if(frame->frameOffset > 5)
	{
		std::cout << std::setw(4) << std::setfill(' ') << "..." << std::endl;
		i = frame->frameOffset - 5;
	}

	for(; i < stack->size(); i++)
	{
		Object* obj = stack->top(i);
		int64 index = int64(i) - frame->frameOffset + 1;
		std::cout << "    " << (index >= 0 ? "+" : "") << index << " ";
		printObject(obj);
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Debugger::printStack()
{
	uint32 i = 0; // TODO
	WorkStack* stack = mVM->getStack();

	std::cout << "[Stack]" << std::endl;

	if(mVM->hasStackFrame())
	{
		StackFrame* frame = mVM->getStackFrame();
		if(frame->frameOffset > 5)
		{
			std::cout << std::setw(4) << std::setfill(' ') << "..." << std::endl;
			i = frame->frameOffset - 5;
		}
	}

	for(; i < stack->size(); i++)
	{
		Object* obj = stack->top(i);
		std::cout << std::setw(4) << std::setfill(' ') << "+" << i << " ";
		printObject(obj);
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void Debugger::printClassMethods(ClassReflection* klass)
{
	std::cout << "[Class " << klass->getName() << "]" << std::endl;
	for(uint16 methodi = 0; methodi < klass->getMethodsCount(); methodi++)
	{
		MethodReflection* method = klass->getMethod(methodi);
		if(method)
		{
			std::cout << std::setw(4) << std::setfill(' ') << "+" << methodi << " " << method->getSelector() << std::endl;
		}
	}
}

void Debugger::started()
{
	if(!isEnabled()) return;
	if(isStepping()) system("CLS");
}

void Debugger::ended()
{
	if(!isEnabled()) return;
}

void Debugger::step(StackFrame* frame)
{
	if(!isEnabled()) return;
	if(isStepping()) system("CLS");
	
	std::cout << std::endl << "--------- STEP ---------" << std::endl;
	printLastOutput();
	//mVM->getHeap()->collect();
	printFrame(frame);

	if(isStepping())
	{
		std::string input;
		std::cout << std::endl << "\r>";
		std::getline(std::cin, input);
		std::cout << "\r";
	}
}

bool Debugger::catchException(StackFrame* frame, const Exception& ex)
{
	std::cout << std::endl << "--------- EXCEPTION ---------" << std::endl;
	printLastOutput();
	printFrame(frame);

	if(ex.getName() == "MethodNotFoundException") // TODO: better
	{
		std::cout << std::endl;
		StackRef<Object> receiver(frame, frame->stackTopIndex());
		ClassReflection* klass = mVM->getClassTable()->translate(receiver);
		printClassMethods(klass);
	}
	
	//std::cout << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;

	return false; // TODO
}