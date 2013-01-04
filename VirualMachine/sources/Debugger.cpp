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
	cout << BEER_WIDEN("[LastOutput]") << '\n';
	Console::getOutput().flush(cout);
	cout << std::endl;
}

void Debugger::printInstruction(const Bytecode::Instruction* instr, uint16 programCounter)
{
	cout /*<< std::endl*/ << BEER_WIDEN("[Instruction]") << std::endl;
	cout << std::setw(4);
	cout << std::setfill(BEER_WIDEN(' ')) << BEER_WIDEN("+") << programCounter << BEER_WIDEN(" ");
	instr->printTranslated(mVM);
	cout << std::endl;
}

void Debugger::printNativeInstruction()
{
	cout /*<< std::endl*/ << "[Instruction]" << std::endl;
	cout << std::setw(4);
	cout << std::setfill(BEER_WIDEN(' ')) << "+" << "NATIVE_CODE ";
	cout << std::endl;
}

void Debugger::printCallStack(Frames* frames)
{
	cout << "[CallStack]" << std::endl;
	
	uint32 i = 0;
	for(Frames::const_iterator it = frames->begin(); it != frames->end(); it++)
	{
		const StackFrame& frame = *it;
		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "+" << i << " ";
		
		if(frame.method)
		{
			cout << frame.method << " " << frame.method->getSelector() << "@" << frame.programCounter;
		}
		else
		{
			cout << "no method";
		}
		
		cout << std::endl;
		i++;
	}
	cout << std::endl;
}

void Debugger::printObject(Object* object)
{
	for(ObjectList::const_iterator it = mPrintedObjects.begin(); it != mPrintedObjects.end(); it++)
	{
		if(*it == object)
		{
			cout << "...";
			return;
		}
	}

	mPrintedObjects.push_back(object);

	if(object == NULL) cout << "null";
	else
	{
		ClassReflection* klass = mVM->getClassTable()->translate(object);
		if(klass)
		{
			if(!klass->isValueType()) cout << "#" << object << " ";

			stringstream ss;
			klass->dump(object, ss);
			cout << ss.str();

			if(klass->getPropertiesCount() > 0)
			{
				cout << " {";
				for(uint32 i = 0; i < klass->getPropertiesCount(); i++)
				{
					PropertyReflection* prop = klass->getProperty(i);
					Object* child = object->getChild<Object>(i);
				
					if(prop) cout << prop->getName() << ": ";

					if(child == NULL)
					{
						cout << "null";
					}
					else
					{
						printObject(child);
					}

					if(i < klass->getPropertiesCount() - 1) cout << ", ";
				}
				cout << "}";
			}
		}
		else
		{
			cout << "#" << object << " has no class";
		}
	}

	mPrintedObjects.pop_back();
}

void Debugger::printFrame(Frames* frames, StackFrame* frame)
{
	printCallStack(frames);
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
	WorkStack* stack = frame->stack;//mVM->getStack();

	cout << "[FrameStack|offset=" << frame->frameOffset << "]" << std::endl;

	if(frame->frameOffset > 5)
	{
		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "..." << std::endl;
		i = frame->frameOffset - 5;
	}

	for(; i < stack->size(); i++)
	{
		Object* obj = stack->top(i);
		int64 index = int64(i) - frame->frameOffset + 1;
		cout << "    " << (index >= 0 ? "+" : "") << index << " ";
		printObject(obj);
		cout << std::endl;
	}
	cout << std::endl;
}

void Debugger::printStack()
{
	uint32 i = 0; // TODO
	WorkStack* stack = mVM->getStack();

	cout << "[Stack]" << std::endl;

	// TODO
	if(false)//mVM->hasStackFrame())
	{
		StackFrame* frame = NULL;//mVM->getStackFrame();
		if(frame->frameOffset > 5)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "..." << std::endl;
			i = frame->frameOffset - 5;
		}
	}

	for(; i < stack->size(); i++)
	{
		Object* obj = stack->top(i);
		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "+" << i << " ";
		printObject(obj);
		cout << std::endl;
	}
	cout << std::endl;
}

void Debugger::printClassMethods(ClassReflection* klass)
{
	cout << "[Class " << klass->getName() << "]" << std::endl;
	for(uint16 methodi = 0; methodi < klass->getMethodsCount(); methodi++)
	{
		MethodReflection* method = klass->getMethod(methodi);
		if(method)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << methodi << " " << method->getSelector() << std::endl;
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

void Debugger::step(Frames* frames, StackFrame* frame)
{
	if(!isEnabled()) return;
	if(isStepping()) system("CLS");
	
	cout << std::endl << "--------- STEP ---------" << std::endl;
	printLastOutput();
	//mVM->getHeap()->collect();
	printFrame(frames, frame);

	if(isStepping())
	{
		string input;
		cout << std::endl << "\r>";
		std::getline(cin, input);
		cout << "\r";
	}
}

bool Debugger::catchException(Frames* frames, StackFrame* frame, const Exception& ex)
{
	cout << std::endl << "--------- EXCEPTION ---------" << std::endl;
	printLastOutput();
	printFrame(frames, frame);

	if(ex.getName() == BEER_WIDEN("MethodNotFoundException")) // TODO: better
	{
		cout << std::endl;
		StackRef<Object> receiver(frame, frame->stackTopIndex());
		ClassReflection* klass = mVM->getClassTable()->translate(receiver);
		printClassMethods(klass);
	}
	
	cout << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;

	return false; // TODO
}