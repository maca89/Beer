#include "stdafx.h"
#include "Debugger.h"
#include "VirtualMachine.h"
#include "Method.h"
#include "Console.h"
#include "Property.h"
#include "Class.h"

using namespace Beer;


Debugger::Debugger(VirtualMachine* vm, GC* gc)
	: mEnabled(false), mStepping(false), Thread(vm, gc)
{
}

Debugger::~Debugger()
{
}

void Debugger::printLastOutput()
{
	cout << BEER_WIDEN("[LastOutput]") << '\n';
	//Console::getOutput().flush(cout); // probably not working, TODO
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

void Debugger::printClassName(StackRef<Class> klass)
{
	Frame* frame = getFrame();

	StackRef<String> klassName(frame, frame->stackPush());
	Class::getName(this, klass, klassName);
	cout << klassName->c_str();
	frame->stackMoveTop(-1); // pop klassName
}

void Debugger::printObjectClassName(StackRef<Object> object)
{
	Frame* frame = getFrame();

	StackRef<Class> klass(frame, frame->stackPush(
		getClass(object)
	));

	printClassName(klass);
	
	frame->stackMoveTop(-1); // pop klass
}

void Debugger::printCalledMethodSignature(Frame* frame, StackRef<Object> receiver, StackRef<Method> method)
{
	cout << "method: ";
	printObjectClassName(receiver);
	cout << "::";
	printMethodSignature(method);
	
	cout << ", vPC: " << frame->getProgramCounter() > 0 ? frame->getProgramCounter() - 1 : 0;

	cout << ", receiver: ";
	printObject(receiver);
}

void Debugger::printMethodSignature(StackRef<Method> method)
{
	Frame* frame = getFrame();

	cout << ((String*)method->getChildren()[Method::CHILD_ID_METHOD_NAME])->c_str();
	cout << "(";
				
	StackRef<Param> arg(frame, frame->stackPush());
	StackRef<String> argName(frame, frame->stackPush());

	for(uint16 parami = 0; parami < method->getParamsCount(); parami++)
	{
		Method::getParam(this, method, arg, parami);
		if(!arg.isNull())
		{
			Param::getName(this, arg, argName);
			cout << argName->c_str();
		}
		else
		{
			cout << "?";
		}
		if(parami < method->getParamsCount() -1)
		{
			cout << ",";
		}
	}

	frame->stackMoveTop(-2); // pop arg, argName

	cout << ")";
	
}

void Debugger::printCallStack(Thread* thread, Frame* frame)
{
	cout << "[CallStack]" << std::endl;

	typedef std::vector<Frame*> FrameVector;
	uint32 framesMax = 5;
	uint32 frameCounter = 0;
	uint32 framei = 0;
	FrameVector myframes(framesMax);

	Frame* frames = thread->getFrames();
	for(framei; framei < frames->stackSize(); framei++)
	{
		Frame* otherFrame = frames->stackTop<Frame>(framei);
		if(otherFrame == frame)
		{
			break;
		}

		// print a couple of first frames
		if(framei < framesMax)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << framei << " ";
			
			StackRef<Method> receiver(otherFrame, -2);
			StackRef<Method> method(otherFrame, -1);
		
			if(!method.isNull())
			{
				printCalledMethodSignature(otherFrame, receiver, method);
			}
			else
			{
				cout << "no method";
			}
			cout << std::endl;
		}
		
		frameCounter = (frameCounter + 1) % framesMax;
		myframes[frameCounter] = otherFrame;
	}

	// print a couple of last frames
	if(framei >= framesMax)
	{
		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "..." << std::endl;

		for(uint32 i = 0; i < framesMax; i++)
		{
			Frame* otherFrame = myframes[(frameCounter + i + 1) % framesMax];
			
			StackRef<Method> receiver(otherFrame, -2);
			StackRef<Method> method(otherFrame, -1);

			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << (framei - i) << " ";
		
			if(!method.isNull())
			{
				printCalledMethodSignature(otherFrame, receiver, method);
			}
			else
			{
				cout << "no method";
			}
			cout << std::endl;
		}
	}

	// print the last frame
	StackRef<Method> receiver(frame, -2);
	StackRef<Method> method(frame, -1);

	cout << std::setw(4);
	cout << std::setfill(BEER_WIDEN(' ')) << "+" << (framei) << " ";
		
	if(!method.isNull())
	{
		printCalledMethodSignature(frame, receiver, method);
	}
	else
	{
		cout << "no method";
	}

	cout << std::endl;
}

void Debugger::printObject(StackRef<Object> object)
{
	Frame* frame = getFrame();

	for(ObjectList::const_iterator it = mPrintedObjects.begin(); it != mPrintedObjects.end(); it++)
	{
		if(*it == *object)
		{
			cout << "...";
			return;
		}
	}

	mPrintedObjects.push_back(*object);

	if(object.isNull()) cout << "null";
	else
	{
		// TODO: use Debugger stack

		cout << "#" << *object << " ";

		StackRef<Class> klass(frame, frame->stackPush(
			getClass(object)
		));

		if(!klass.isNull())
		{
			//if(!klass->isValueType()) cout << "#" << *object << " ";

			StackRef<Method> toStringMethod(frame, frame->stackPush());

			StackRef<String> selector(frame, frame->stackPush());
			createString(selector, BEER_WIDEN("Object::String()")); // TODO: selector in a pool

			Class::findMethod(this, klass, selector, toStringMethod);
			frame->stackMoveTop(-1); // pop selector

			// there should be a method, so no check for NULL
			if(toStringMethod.isNull())
			{
				cout << "*no Object::String() method*";
				frame->stackMoveTop(-1);
				goto end;
			}

			// call Object::String()
			if(true)
			{
				StackRef<String> result(frame, frame->stackPush());
				frame->stackPush(*object);
				frame->stackPush(*toStringMethod);
				openFrame();
				toStringMethod->call(this);
				//closeFrame(); // pops copied object, copied method

				cout << result->c_str();

				frame->stackMoveTop(-1); // pop result
			}

			// just print class name
			else
			{
				printClassName(klass);
			}

			frame->stackMoveTop(-1); // pop toStringMethod

			if(klass->getPropertiesCount() > 0)
			{
				cout << " {";
				for(uint32 i = 0; i < klass->getPropertiesCount(); i++)
				{
					// TODO
					/*Property* prop = klass->getProperty(i);
					Object* child = object->_getChild<Object>(i);
				
					if(prop) cout << prop->getName() << ": ";

					if(child == NULL)
					{
						cout << "null";
					}
					else
					{
						printObject(child);
					}

					if(i < klass->getPropertiesCount() - 1) cout << ", ";*/
				}
				cout << "}";
			}
		}
		else
		{
			cout << "*no class*";
		}

end:
		frame->stackMoveTop(-1); // pop class
	}

	mPrintedObjects.pop_back();
}

void Debugger::printFrame(Thread* thread, Frame* frame)
{
	printCallStack(thread, frame);
	printFrameStack(frame);

	StackRef<Method> method(frame, -1);

	if(method.isNull())
	{
		cout << "[No method]" << std::endl;
	}
	else if(method->isBytecode())
	{
		//const Bytecode::Instruction* instr = frame->method->getBytecode()->getInstruction(frame->programCounter);
		//const Bytecode::Instruction* instr = reinterpret_cast<const Bytecode::Instruction*>(frame->getInstructionPointer());
		//printInstruction(instr, frame->getProgramCounter());
	}
	else
	{
		printNativeInstruction(/*frame*/);
	}
}

void Debugger::printFrameStack(Frame* frame)
{
	cout << "[FrameStack]" << std::endl;

	StackRef<Method> method(frame, -1);
	if(method.isNull())
	{
		for(uint32 i = 0; i < frame->stackSize(); i++)
		{
			StackRef<Object> object(frame, frame->translate(i));

			int64 index = int64(i) - frame->getFrameOffset() + 1;
			cout << "    " << (index >= 0 ? "+" : "") << index << " ";
			printObject(object);
			cout << std::endl;
		}
	}
	else
	{
		uint16 returnsCount = method->getReturnsCount();
		uint16 argsCount = method->getParamsCount();

		for(uint16 reti = returnsCount; reti >= 1; reti--)
		{
			StackRef<Object> object(frame, -reti - argsCount - 2);

			cout << "    " << "ret" << reti << ": ";
			printObject(object);
			cout << std::endl;
		}

		for(uint16 argi = argsCount; argi >= 1; argi--)
		{
			StackRef<Object> object(frame, -argi - 2);

			cout << "    " << "arg" << argi << ": ";
			printObject(object);
			cout << std::endl;
		}
		
		StackRef<Object> receiver(frame, -2);
		cout << "    " << "receiver" << ": ";
		printObject(receiver);
		cout << std::endl;

		cout << "    " << "method" << ": ";
		printObject(method);
		cout << std::endl;

		uint32 vari = 0;
		for(uint32 i = returnsCount + argsCount + 2; i < frame->stackSize(); i++)
		{
			StackRef<Method> object(frame, frame->translate(i));

			int64 index = int64(i) - frame->getFrameOffset() + 1;
			cout << "    var" << vari << ": ";
			printObject(object);
			cout << std::endl;
			vari++;
		}
	}


	//cout << std::endl;
}

void Debugger::printClassMethods(Class* klass)
{
	cout << "[Class " << ((String*)klass->getChildren()[Class::CHILD_ID_CLASS_NAME])->c_str() << "]" << std::endl;
	for(uint16 methodi = 0; methodi < klass->getMethodsCount(); methodi++)
	{
		/*Pair* definedMethod = klass->getMethod(methodi);
		if(definedMethod)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << methodi << " NOT IMPLEMENTED" << definedMethod->_getFirst<String>()->c_str() << std::endl;// TODO: selector
		}*/
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

void Debugger::step(Thread* thread, Frame* frame)
{
	if(!isEnabled()) return;
	if(isStepping()) system("CLS");
	
	cout << std::endl << "--------- STEP ---------" << std::endl;
	printLastOutput();
	//mVM->getHeap()->collect();
	printFrame(thread, frame);

	if(isStepping())
	{
		string input;
		cout << std::endl << "\r>";
		std::getline(cin, input);
		cout << "\r";
	}
}

bool Debugger::catchException(Thread* thread, Frame* frame, const Exception& ex)
{
	cout << std::endl << "--------- EXCEPTION ---------" << std::endl;
	printLastOutput();
	printFrame(thread, frame);

	if(ex.getName().compare(BEER_WIDEN("MethodNotFoundException")) == 0) // TODO: better
	{
		cout << std::endl;
		StackRef<Object> receiver(frame, frame->stackTopIndex());
		Class* klass = ((Thread*)mVM)->getClass(receiver);
		printClassMethods(klass);
	}
	
	cout << "\n[" << ex.getName() << "]\n" << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;

	return false; // TODO
}