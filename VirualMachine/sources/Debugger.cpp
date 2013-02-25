#include "stdafx.h"
#include "Debugger.h"
#include "VirtualMachine.h"
#include "MethodReflection.h"
#include "ConsoleClass.h"
#include "PropertyReflection.h"
#include "Class.h"

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

void Debugger::printCallStack(StackFrame* frame)
{
	cout << "[CallStack]" << std::endl;

	typedef std::vector<StackFrame*> FrameVector;
	uint32 framesMax = 10;
	uint32 frameCounter = 0;
	uint32 framei = 0;
	FrameVector frames(framesMax);
	
	while(frame)
	{
		if(framei < framesMax)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << framei << " ";
		
			if(frame->method)
			{
				cout << frame->method << " " << frame->method->getName() << "@" << frame->programCounter;// TODO: selector
			}
			else
			{
				cout << "no method";
			}
			cout << std::endl;
		}

		frameCounter = (frameCounter + 1) % framesMax;
		frames[frameCounter] = frame;

		framei++;
		frame = frame->prev;
	}

	if(framei >= framesMax)
	{
		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "..." << std::endl;

		for(uint32 i = 0; i < framesMax; i++)
		{
			frame = frames[(frameCounter + i + 1) % framesMax];

			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << (framei - i) << " ";
		
			if(frame->method)
			{
				cout << frame->method << " " << frame->method->getName() << "@" << frame->programCounter;// TODO: selector
			}
			else
			{
				cout << "no method";
			}
			cout << std::endl;
		}
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
		Class* klass = mVM->getClass(object);
		if(klass)
		{
			if(!klass->isValueType()) cout << "#" << object << " ";

			stringstream ss;
			//klass->dump(object, ss); // TODO: call String()
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

void Debugger::printFrame(StackFrame* frame)
{
	printCallStack(frame);
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

void Debugger::printClassMethods(Class* klass)
{
	cout << "[Class " << klass->getName() << "]" << std::endl;
	for(uint16 methodi = 0; methodi < klass->getMethodsCount(); methodi++)
	{
		Pair* definedMethod = klass->getMethod(methodi);
		if(definedMethod)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << methodi << " " << definedMethod->getFirst<String>()->c_str() << std::endl;// TODO: selector
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
	
	cout << std::endl << "--------- STEP ---------" << std::endl;
	printLastOutput();
	//mVM->getHeap()->collect();
	printFrame(frame);

	if(isStepping())
	{
		string input;
		cout << std::endl << "\r>";
		std::getline(cin, input);
		cout << "\r";
	}
}

bool Debugger::catchException(StackFrame* frame, const Exception& ex)
{
	cout << std::endl << "--------- EXCEPTION ---------" << std::endl;
	printLastOutput();
	printFrame(frame);

	if(ex.getName() == BEER_WIDEN("MethodNotFoundException")) // TODO: better
	{
		cout << std::endl;
		StackRef<Object> receiver(frame, frame->stackTopIndex());
		Class* klass = mVM->getClass(receiver);
		printClassMethods(klass);
	}
	
	cout << "\n[" << ex.getName() << "]\n" << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;

	return false; // TODO
}