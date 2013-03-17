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

void Debugger::printInstruction(Bytecode* bc, const Bytecode::Instruction* instr, uint16 programCounter)
{
	cout /*<< std::endl*/ << BEER_WIDEN("[Instruction]") << std::endl;
	cout << std::setw(4);
	cout << std::setfill(BEER_WIDEN(' ')) << BEER_WIDEN("+") << programCounter << BEER_WIDEN(" ");
	bc->printTranslatedInstruction(this, instr);
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
	frame->stackMoveTop(-1); // pop klassName
}

void Debugger::printObjectClassName(StackRef<Object> object)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush(
		getType(object)
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
	BEER_STACK_CHECK();

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
				StackRef<String> result(frame, frame->stackPush());
				frame->stackPush(*object);
				frame->stackPush(*toStringMethod);
				openFrame();
				toStringMethod->call(this); // pops copied object, copied method

				cout << result->c_str();

				frame->stackMoveTop(-2); // pop result, toStringMethod
			}

			StackRef<Integer> propertiesCount(frame, frame->stackPush());
			Class::getPropertiesCount(this, klass, propertiesCount);

			if(propertiesCount->getData() > 0)
			{
				StackRef<Integer> index(frame, frame->stackPush());

				cout << " {";
				for(uint32 i = 0; i < propertiesCount->getData(); i++)
				{
					createInteger(index, i);

					StackRef<Property> prop(frame, frame->stackPush());
					Class::getProperty(this, klass, index, prop);

					if(!prop.isNull())
					{
						// print name
						{
							StackRef<String> name(frame, frame->stackPush());
							Property::getName(this, prop, name);

							cout << name->c_str() << ": ";
							frame->stackMoveTop(-1); // pop name
						}

						// print child
						{
							createInteger(index, Object::OBJECT_CHILDREN_COUNT + i);

							StackRef<Object> child(frame, frame->stackPush());
							Object::getChild(this, object, index, child);

							printObject(child);
							frame->stackMoveTop(-1); // pop child
						}

						if(i < propertiesCount->getData() - 1) cout << ", ";
					}

					frame->stackMoveTop(-1); // pop prop
				}
				cout << "}";

				frame->stackMoveTop(-1); // pop index
			}

			frame->stackMoveTop(-1); // pop propertiesCount
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

void Debugger::printClassMethods(StackRef<Class> klass)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Integer> methodsCount(frame, frame->stackPush());
	Class::getMethodsCount(this, klass, methodsCount);

	cout << "[Class " << ((String*)klass->getChildren()[Class::CHILD_ID_CLASS_NAME])->c_str() << "]" << std::endl;
	for(uint16 methodi = 0; methodi < methodsCount->getData(); methodi++)
	{
		/*Pair* definedMethod = klass->getMethod(methodi);
		if(definedMethod)
		{
			cout << std::setw(4);
			cout << std::setfill(BEER_WIDEN(' ')) << "+" << methodi << " NOT IMPLEMENTED" << definedMethod->_getFirst<String>()->c_str() << std::endl;// TODO: selector
		}*/
	}

	frame->stackMoveTop(-1); // pop methodsCount
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

void Debugger::printBytecodeMethods(Class* c)
{
	Frame* frame = getFrame();
	BEER_STACK_CHECK();

	StackRef<Class> klass(frame, frame->stackPush(c));

	string klassName;

	// get classname
	{
		StackRef<String> name(frame, frame->stackPush());
		Class::getName(this, klass, name);
		klassName = name->c_str();
		klassName += BEER_WIDEN("::");
		frame->stackMoveTop(-1); // pop name
	}

	StackRef<Integer> methodsCount(frame, frame->stackPush());
	Class::getMethodsCount(this, klass, methodsCount);

	StackRef<Integer> index(frame, frame->stackPush());
	StackRef<Pair> pair(frame, frame->stackPush());

	for(Integer::IntegerData i = 0; i < methodsCount->getData(); i++)
	{
		createInteger(index, i);
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

					frame->stackMoveTop(-1); // pop selector
				}

				if(!parentOrOverriding)
				{
					printBytecodeMethod(klass, method);
				}
			}
			
			frame->stackMoveTop(-1); // method
		}
	}

	frame->stackMoveTop(-4); // pop klass, methodsCount, index, pair
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
		frame->stackMoveTop(-1); // pop name
	}

	// print method name
	{
		StackRef<String> name(frame, frame->stackPush());
		Method::getName(this, method, name);
		cout << name->c_str();
		frame->stackMoveTop(-1); // pop name
	}

	cout << "\n{";

	// print bytecode
	{
		printBytecode(method->getBytecode());
	}

	cout << "}\n";
}

void Debugger::printBytecode(Bytecode* bc)
{
	for(uint16 i = 0; i < bc->getInstructionCount(); i++)
	{
		const Bytecode::Instruction* instr = bc->getInstruction(i);
		cout << "\t";
		bc->printTranslatedInstruction(this, instr);
		cout << "\n";
	}
}