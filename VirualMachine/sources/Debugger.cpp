#include "stdafx.h"
#include "Debugger.h"
#include "VirtualMachine.h"
#include "Method.h"
#include "Console.h"
#include "Property.h"
#include "Class.h"
#include "Pair.h"
#include "FrameInspector.h"

using namespace Beer;


Debugger::Debugger(VirtualMachine* vm, GenerationalGC* gc)
	: mEnabled(false), mStepping(false), Thread(999, vm, gc)
{
}

Debugger::~Debugger()
{
}

void Debugger::init()
{
	this->Thread::init();
	
	mTemporaryContext.init(mHeap, getVM()->getFrameClass());
	setContext(&mTemporaryContext);
}

void Debugger::printLastOutput()
{
	cout << BEER_WIDEN("[LastOutput]") << '\n';
	//Console::getOutput().flush(cout); // probably not working, TODO
	cout << std::endl;
}

void Debugger::printInstruction(StackRef<Method> method, Bytecode* bc, Bytecode::Instruction* instr, uint16 programCounter)
{
	cout /*<< std::endl*/ << BEER_WIDEN("[Instruction]") << std::endl;
	cout << std::setw(4);
	cout << std::setfill(BEER_WIDEN(' ')) << BEER_WIDEN("+") << programCounter << BEER_WIDEN(" ");
	bc->printTranslatedInstruction(this, *method, reinterpret_cast<byte*>(instr));
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
	BEER_STACK_CHECK();

	StackRef<String> klassName(frame, frame->stackPush());
	Class::getName(this, klass, klassName);
	cout << klassName->c_str();
	frame->stackPop(); // pop klassName
}

void Debugger::printObjectClassName(StackRef<Object> object)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush(
		getType(object)
	));

	printClassName(klass);
	
	frame->stackPop(); // pop klass
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
	BEER_STACK_CHECK();

	cout << method->getName()->c_str();
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

void Debugger::printCallStack(Thread* thread, Frame* topFrame)
{
	cout << "[CallStack]" << std::endl;

	FrameInspector insp;

	int64 framei = 0;

	// find frame index
	insp.start(topFrame);
	while(insp.hasFrame())
	{
		framei++;
		insp.nextFrame();
	}

	// not a top frame
	if(framei != thread->getThreadContext()->getFramesCount() - 1)
	{
		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "...\n";
	}

	// print call stack
	insp.start(topFrame);
	while(insp.hasFrame())
	{
		Frame* frame = insp.getFrame();

		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "+" << framei << " ";

		StackRef<Method> receiver(frame, Frame::INDEX_RECEIVER);
		StackRef<Method> method(frame, Frame::INDEX_METHOD);

		if(!method.isNull())
		{
			printCalledMethodSignature(frame, receiver, method);
		}
		else
		{
			cout << "no method";
		}
		cout << "\n";

		framei--;
		insp.nextFrame();
	}


	return;

	{
		typedef std::vector<Frame*> FrameVector;
		uint32 framesMax = 5;
		uint32 frameCounter = 0;
		uint32 framei = 0;
		FrameVector myframes(framesMax);

		//cout << "*Not implemented*\n";
		return;

		Frame* frames = NULL;//thread->getThreadContext()->getRootFrame();
		for(framei; framei < frames->stackLength(); framei++)
		{
			Frame* otherFrame = frames->stackTop<Frame>(framei);
			if(otherFrame == topFrame)
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
		StackRef<Method> receiver(topFrame, -2);
		StackRef<Method> method(topFrame, -1);

		cout << std::setw(4);
		cout << std::setfill(BEER_WIDEN(' ')) << "+" << (framei) << " ";
		
		if(!method.isNull())
		{
			printCalledMethodSignature(topFrame, receiver, method);
		}
		else
		{
			cout << "no method";
		}

		cout << std::endl;
	}
}

void Debugger::printObject(Object* object)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Object> oo(frame, frame->stackPush(object));
	printObject(oo);
	frame->stackPop(); // pop oo
}

void Debugger::printObject(StackRef<Object> object)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

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
			getType(object)
		));

		if(!klass.isNull())
		{
			//if(!klass->isValueType()) cout << "#" << *object << " ";

			// is string
			if(*klass == getVM()->getStringClass())
			{
				cout << BEER_WIDEN("\"") << object.staticCast<String>()->c_str()  << BEER_WIDEN("\""); // TODO: print just a first fe chars
			}
			else // to string
			{
				/*if(!klass->isValueType())
				{
					printClassName(klass);
				}*/

				StackRef<Method> toStringMethod(frame, frame->stackPush());

				StackRef<String> selector(frame, frame->stackPush());
				createString(selector, BEER_WIDEN("Object::String()")); // TODO: selector in a pool

				toStringMethod = klass->findVirtualMethod(*selector); // TODO
				frame->stackPop(); // pop selector

				// there should be a method, so no check for NULL
				if(toStringMethod.isNull())
				{
					cout << "*no Object::String() method*";
					frame->stackPop();
					goto end;
				}

				// call Object::String()
				StackRef<String> result(frame, frame->stackPush());
				frame->stackPush(*object);
				frame->stackPush(*toStringMethod);
				openFrame();
				toStringMethod->invoke(this); // pops copied object, copied method

				cout << result->c_str();

				frame->stackMoveTop(-2); // pop result, toStringMethod
			}

			if(klass->getPropertiesCount() > 0)
			{
				cout << " {";
				for(uint32 i = 0; i < klass->getPropertiesCount(); i++)
				{
					Property* prop = klass->getProperty(i);

					if(prop)
					{
						// print name
						cout << prop->getPropertyName()->c_str() << ": ";

						// print child
						{
							StackRef<Object> child(frame, frame->stackPush());
							Object::getChild(this, object, Object::OBJECT_CHILDREN_COUNT + i, child);

							printObject(child);
							frame->stackPop(); // pop child
						}

						if(i < klass->getPropertiesCount() - 1) cout << ", ";
					}
				}
				cout << "}";
			}
		}
		else
		{
			cout << "*no class*";
		}

end:
		frame->stackPop(); // pop class
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
		for(uint32 i = 0; i < frame->stackLength(); i++)
		{
			StackRef<Object> object(frame, frame->translateAbsolute(i));

			int64 index = int64(i) /*- frame->getFrameOffset()*/ + 1;
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
		for(uint32 i = /*returnsCount + argsCount + 2*/0; i < frame->stackLength(); i++)
		{
			StackRef<Method> object(frame, frame->translateAbsolute(i));

			int64 index = int64(i) /*- frame->getFrameOffset()*/ + 1;
			cout << "    var" << vari << ": ";
			printObject(object);
			cout << std::endl;
			vari++;
		}
	}


	//cout << std::endl;
}

void Debugger::printClassMethods(StackRef<Class> klassOnStack)
{
	Class* klass = *klassOnStack;

	cout << "[Class " << klass->getName()->c_str() << "]" << std::endl;
	for(uint32 methodi = 0; methodi < klass->getMethodsCount(); methodi++)
	{
		// TODO
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
	Frame* myframe = getFrame();
	BEER_STACK_CHECK();

	cout << std::endl << "--------- EXCEPTION ---------" << std::endl;
	printLastOutput();
	printFrame(thread, frame);

	if(ex.getName().compare(BEER_WIDEN("MethodNotFoundException")) == 0) // TODO: better
	{
		cout << std::endl;

		StackRef<Object> receiver(myframe, myframe->stackPush(
			frame->stackTop()
		));

		NULL_ASSERT(*receiver); // TODO

		StackRef<Class> klass(myframe, myframe->stackPush());
		getType(receiver, klass);

		printClassMethods(klass);
		
		myframe->stackMoveTop(-2);  //pop receiver & klass
	}
	
	cout << "\n[" << ex.getName() << "]\n" << ex.getMessage() << " @" << ex.getFilename() << ":" << ex.getFileline() << std::endl;

	return false; // TODO
}

void Debugger::printBytecodeMethods(Class* klass)
{
	//TODO

	/*string klassName = klass->getName()->c_str();
	klassName += BEER_WIDEN("::");

	for(Integer::IntegerData i = 0; i < klass->getMethodsCount(); i++)
	{
		Class::getMethod(this, klass, index, pair);

		if(!pair.isNull())
		{
			StackRef<Method> method(frame, frame->stackPush());
			Pair::getSecond(this, pair, method);

			if(method->isBytecode())
			{
				bool parentOrOverriding = false;

				// is overriding?
				{
					StackRef<String> selector(frame, frame->stackPush());
					Pair::getFirst(this, pair, selector);

					string cselector = selector->c_str();
					size_t pos = cselector.find(klassName);
					parentOrOverriding = pos != 0;//klassName.size(); // ugly, TODO

					frame->stackPop(); // pop selector
				}

				if(!parentOrOverriding)
				{
					printBytecodeMethod(klass, method);
				}
			}
		}
	}*/
}

void Debugger::printBytecodeMethod(StackRef<Class> klass, StackRef<Method> method)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	// print class name
	{
		StackRef<String> name(frame, frame->stackPush());
		Class::getName(this, klass, name);
		cout << name->c_str() << "::";
		frame->stackPop(); // pop name
	}

	// print method name
	{
		StackRef<String> name(frame, frame->stackPush());
		Method::getName(this, method, name);
		cout << name->c_str();
		frame->stackPop(); // pop name
	}

	cout << "\n{";

	// print bytecode
	{
		printBytecode(method, method->getBytecode());
	}

	cout << "}\n";
}

void Debugger::printBytecode(StackRef<Method> method, Bytecode* bc)
{
	cout << "*NOT IMPLEMENTED*";
	//TODO
	/*for(uint16 i = 0; i < bc->getInstructionCount(); i++)
	{
		const Bytecode::Instruction* instr = bc->getInstruction(i);
		cout << "\t";
		bc->printTranslatedInstruction(this, *method, instr);
		cout << "\n";
	}*/
}