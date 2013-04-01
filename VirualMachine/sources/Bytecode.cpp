#include "stdafx.h"
#include "Bytecode.h"
#include "VirtualMachine.h"
#include "Object.h"
#include "String.h"
#include "Boolean.h"
#include "Integer.h"
#include "Float.h"
#include "Console.h"
#include "Method.h"
#include "BytecodeInputStream.h"
#include "BytecodeOutputStream.h"
#include "PolymorphicCache.h"
#include "Debugger.h"
#include "Array.h"
#include "GenerationalGC.h"
#include "StringDescriptor.h"
#include "ClassFileDescriptor.h"

#include "FrameInspector.h"

using namespace Beer;

#define BEER_BC_DISPATCH_SWITCH 0
#define BEER_BC_DISPATCH_DIRECT 1

#define BEER_BC_DISPATCH	BEER_BC_DISPATCH_SWITCH


uint16 Bytecode::Instruction::printRaw(const ClassFileDescriptor* classFile) const
{
	uint16 size = sizeof(uint8); // opcode

	switch (opcode)
	{
	case BEER_INSTR_NOP:
		cout << "NOP";
		break;

	case BEER_INSTR_JMP:
		cout << "JMP " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_POP:
		cout << "POP";
		break;

	case BEER_INSTR_TOP:
		cout << "TOP " << getData<int16>();
		size += sizeof(int16);
		break;

	case BEER_INSTR_STORE:
		cout << "STORE " << getData<int16>();
		size += sizeof(int16);
		break;

	case BEER_INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData<int16>();
		size += sizeof(int16);
		break;

	case BEER_INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData<int8>());
		size += sizeof(int8);
		break;

	case BEER_INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData<int32>();
		size += sizeof(int32);
		break;

	case BEER_INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData<int64>();
		size += sizeof(int64);
		break;

	case BEER_INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData<float64>();
		size += sizeof(float64);
		break;

	case BEER_INSTR_PUSH_STRING:
		cout << "PUSH_STRING " << getData<uint32>();
		cout << " // \"" << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str() << "\"";
		size += sizeof(uint32);
		break;

	case BEER_INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16();
		//size += sizeof(uint16);
		cout << "PUSH_CHAR " << getData<uint8>();
		size += sizeof(uint8);
		break;

	case BEER_INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData<uint8>());
		size += sizeof(uint8);
		break;

	case BEER_INSTR_NEW:
		cout << "NEW " << getData<uint32>();
		cout << " // " << classFile->getClassName(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;

	case BEER_INSTR_CLONE:
		cout << "CLONE";
		break;

	case BEER_INSTR_ASSIGN:
		cout << "ASSIGN " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_LOAD:
		cout << "LOAD " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case BEER_INSTR_VIRTUAL_INVOKE:
		cout << "VIRTUAL_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_INTERFACE_INVOKE:
		cout << "INTERFACE_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_SPECIAL_INVOKE:
		cout << "SPECIAL_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case BEER_INSTR_STACK_INVOKE:
		cout << "STACK_INVOKE";
		break;
	
	case BEER_INSTR_RETURN:
		cout << "RETURN " << getData<uint16>();
		size += sizeof(uint16);
		break;
	
	default:
		cout << "[UNKNOWN]";
		break;
	}

	return size;
}

void Bytecode::printTranslatedInstruction(Thread* thread, Method* method, const Instruction* instr)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	switch (instr->opcode)
	{
	case BEER_INSTR_NOP:
		cout << "NOP";
		break;

	case BEER_INSTR_JMP:
		cout << "JMP " << instr->getData<uint16>();
		break;

	case BEER_INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << instr->getData<uint16>();
		break;

	case BEER_INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << instr->getData<uint16>();
		break;

	case BEER_INSTR_POP:
		cout << "POP";
		break;

	case BEER_INSTR_TOP:
		cout << "TOP " << instr->getData<int16>();
		break;

	case BEER_INSTR_STORE:
		cout << "STORE " << instr->getData<int16>();
		break;

	case BEER_INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << instr->getData<int16>();
		break;

	case BEER_INSTR_PUSH_INT64:
		{
			StackRef<Integer> value(frame, frame->stackPush());
			method->loadFromPool(thread, instr->getData<int16>(), value);

			cout << "PUSH_INT64 " << value->getData();
			frame->stackPop(); // pop value
		}
		break;

	case BEER_INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << instr->getData<float64>();
		break;

	case BEER_INSTR_PUSH_STRING:
		{
			StackRef<String> value(frame, frame->stackPush());
			method->loadFromPool(thread, instr->getData<int16>(), value);

			cout << "PUSH_STRING \"" << value->c_str() << "\"";
			frame->stackPop(); // pop value
		}
		break;

	case BEER_INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16(); // TODO
		cout << "PUSH_CHAR " << instr->getData<uint8>(); // TODO
		break;

	case BEER_INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(instr->getData<uint8>());
		break;

	case BEER_INSTR_NEW: // ugly, TODO
		{
			StackRef<String> name(frame, frame->stackPush());

			StackRef<Class> klass(frame, frame->stackPush());
			method->loadFromPool(thread, instr->getData<int16>(), klass);

			Class::getName(thread, klass, name);

			cout << "NEW " << name->c_str();
			frame->stackMoveTop(-2); // pop class, name
		}
		break;

	case BEER_INSTR_CLONE:
		cout << "CLONE";
		break;

	case BEER_INSTR_ASSIGN:
		cout << "ASSIGN " << instr->getData<uint16>();
		break;

	case BEER_INSTR_LOAD:
		cout << "LOAD " << instr->getData<uint16>();
		break;

	case BEER_INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << instr->getData<uint16>();
		break;

	case BEER_INSTR_VIRTUAL_INVOKE:
		{
			StackRef<String> selector(frame, frame->stackPush());
			method->loadFromPool(thread, instr->getData<uint16>(), selector);

			cout << "VIRTUAL_INVOKE \"" << selector->c_str() << "\"";

			frame->stackPop(); // pop selector
		}
		break;
	
	case BEER_INSTR_INTERFACE_INVOKE:
		{
			StackRef<String> selector(frame, frame->stackPush());
			method->loadFromPool(thread, instr->getData<uint16>(), selector);

			cout << "INTERFACE_INVOKE \"" << selector->c_str() << "\"";
			
			frame->stackPop(); // pop selector
		}
		break;
	
	case BEER_INSTR_SPECIAL_INVOKE:
		{
			StackRef<String> selector(frame, frame->stackPush());
			method->loadFromPool(thread, instr->getData<uint16>(), selector);

			cout << "SPECIAL_INVOKE \"" << selector->c_str() << "\"";
			
			frame->stackPop(); // pop selector
		}
		break;

	case BEER_INSTR_STACK_INVOKE:
		cout << "STACK_INVOKE";
		break;
	
	case BEER_INSTR_RETURN:
		cout << "RETURN " << instr->getData<uint16>();
		break;
		
	case BEER_INLINE_BOOLEAN_EQUAL:
		cout << "INLINE_BOOLEAN_EQUAL";
		break;

	case BEER_INLINE_BOOLEAN_NOT_EQUAL:
		cout << "INLINE_BOOLEAN_NOT_EQUAL";
		break;

	case BEER_INLINE_BOOLEAN_OR:
		cout << "INLINE_BOOLEAN_OR";
		break;

	case BEER_INLINE_BOOLEAN_AND:
		cout << "INLINE_BOOLEAN_AND";
		break;

	case BEER_INLINE_BOOLEAN_NEGATION:
		cout << "INLINE_BOOLEAN_NEGATION";
		break;

	case BEER_INLINE_INTEGER_PLUS:
		cout << "INLINE_INTEGER_PLUS";
		break;

	case BEER_INLINE_INTEGER_MINUS:
		cout << "INLINE_INTEGER_MINUS";
		break;

	case BEER_INLINE_INTEGER_MUL:
		cout << "INLINE_INTEGER_MUL";
		break;

	case BEER_INLINE_INTEGER_EQUAL:
		cout << "INLINE_INTEGER_EQUAL";
		break;

	case BEER_INLINE_INTEGER_NOT_EQUAL:
		cout << "INLINE_INTEGER_NOT_EQUAL";
		break;

	case BEER_INLINE_INTEGER_SMALLER:
		cout << "INLINE_INTEGER_SMALLER";
		break;

	case BEER_INLINE_INTEGER_SMALLER_EQUAL:
		cout << "INLINE_INTEGER_SMALLER_EQUAL";
		break;

	case BEER_INLINE_INTEGER_GREATER:
		cout << "INLINE_INTEGER_GREATER";
		break;

	case BEER_INLINE_INTEGER_GREATER_EQUAL:
		cout << "INLINE_INTEGER_GREATER_EQUAL";
		break;

	case BEER_INLINE_ARRAY_GET_LENGTH:
		cout << "INLINE_ARRAY_GET_LENGTH";
		break;

	/*case BEER_INLINE_ARRAY_GET_ITEM:
		cout << "INLINE_ARRAY_GET_ITEM";
		break;

	case BEER_INLINE_ARRAY_SET_ITEM:
		cout << "INLINE_ARRAY_SET_ITEM";
		break;*/

	case BEER_OPTIMAL_ARRAY_ALLOC:
		cout << "OPTIMAL_ARRAY_ALLOC";
		break;
	
	default:
		cout << "[UNKNOWN]";
		break;
	}
}

void DefaultBytecodeCompiler::compile(Thread* thread, StackRef<Method> method, const TemporaryBytecode& bc)
{
	for(uint16 instri = 0; instri < bc.instrCount; instri++)
	{
		uint16 instrIndex = bc.dict[instri];
		Bytecode::Instruction* instr = reinterpret_cast<Bytecode::Instruction*>(&bc.data[instrIndex]);
		Bytecode::OpCode opcode = instr->getOpcode();

		switch(opcode)
		{
			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
				{
					uint16 index = bc.dict[instr->getData<uint16>()];
					int16 offset = static_cast<int16>(static_cast<int32>(index) - static_cast<int32>(instrIndex));
					instr->setData<int16>(offset);
				}
				break;
		}
	}

	method->setBytecode(new Bytecode(bc.data, bc.dataLength));

	// TODO
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	BytecodeInputStream istream(data, dataLength);
	BytecodeOutputStream ostream(instrCount);

	ostream.setAlignMemory(true);

	while(!istream.end())
	{
		ostream.next();

		Bytecode::OpCode opcode = istream.read<Bytecode::OpCode>();
		//cout << i++ << "\t" << opcode << "\t"; builder.getOldInstruction()->printRaw(classFile); cout << "\n";

		switch(opcode)
		{
			// 0 bytes = 0bit

			case BEER_INSTR_NOP:
			case BEER_INSTR_POP:
			case BEER_INSTR_CLONE:
			case BEER_FILL_OPCODE_TABLE:
				ostream.write((void*)LabelTable[opcode]);
				break;

			// 1 byte = 8bit

			case BEER_INSTR_PUSH_BOOL:
			case BEER_INSTR_PUSH_CHAR:// TODO: 16bit
				ostream.write((void*)LabelTable[opcode]);
				ostream.write(istream.read<uint8>());
				break;

			// 2 bytes = 16bit

			case BEER_INSTR_TOP:
			case BEER_INSTR_STORE:
				{
					ostream.write((void*)LabelTable[opcode]);
					int16 value = istream.read<int16>();
					if(value > 0) ostream.write<int16>(value - 1); // -1 because local vars start at 0 (not at 1)
					else ostream.write<int16>(value - 2); // -2 because arguments start at -1 (not at 0) and there is method added ad -1 arg
				}
				break;

			case BEER_INSTR_RETURN:
				{
					ostream.write((void*)LabelTable[opcode]);
					uint16 value = istream.read<uint16>();
					ostream.write<uint16>(0);//static_cast<uint16>(value - method->getParamsCount() - 1));
				}
				break;

			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
			case BEER_INSTR_MOVE_TOP:
			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_LOAD:
			case BEER_INSTR_ASSIGN:
				BEER_BC_SAVE_OPCODE(opcode); todo: ostream.write((void*)LabelTable[opcode]);
				ostream.write(istream.read<uint16>());
				break;

			// 4 bytes = 32bit

			case BEER_INSTR_PUSH_STRING:
				{
					BEER_BC_SAVE_OPCODE(opcode);
					// TODO: load from classfile pool
					const char16* cstring = classFile->getDescriptor<StringDescriptor>(istream.read<int32>())->c_str();
					Reference<String> str = String::gTranslate(thread, cstring);
					
					StackRef<String> value(frame, frame->stackPush(
						*str
					));

					ostream.write<uint16>(bc->storeToPool(thread, value));
					frame->stackPop(); // pop value
				}
				break;

			case BEER_INSTR_NEW:
				{
					const char16* cstring = classFile->getClassName(istream.read<int32>())->c_str();
#ifdef BEER_INLINE_OPTIMALIZATION
					if(strcmp(cstring, BEER_WIDEN("Array")) == 0)
					{
						BEER_BC_SAVE_OPCODE(BEER_OPTIMAL_ARRAY_ALLOC);
					}
					else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						BEER_BC_SAVE_OPCODE(opcode);

						StackRef<Class> klass(frame, frame->stackPush());

						StackRef<String> name(frame, frame->stackPush(
							*String::gTranslate(thread, cstring)
						));

						thread->findClass(name, klass);

						ostream.write<uint16>(bc->storeToPool(thread, klass.staticCast<Object>()));

						frame->stackMoveTop(-2); // pop name, klass
					}
				}
				break;

			case BEER_INSTR_VIRTUAL_INVOKE:
			case BEER_INSTR_SPECIAL_INVOKE:
				{
					const char16* cselector = classFile->getDescriptor<StringDescriptor>(istream.read<int32>())->c_str();
					Reference<String> refselector = String::gTranslate(thread, cselector);
#ifdef BEER_INLINE_OPTIMALIZATION
					Bytecode::OpCode newOpcode = thread->getVM()->getInlineFunctionTable()->find(refselector.get());
					if(newOpcode != BEER_INSTR_NOP)
					{
						BEER_BC_SAVE_OPCODE(newOpcode);
					}
					else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						BEER_BC_SAVE_OPCODE(opcode);
						
						StackRef<String> selector(frame, frame->stackPush(
							*refselector
						));
						
						ostream.write<uint16>(bc->storeToPool(thread, selector));
						frame->stackPop(); // pop selector
					}
				}
				break;
				
			// no caching
			case BEER_INSTR_INTERFACE_INVOKE:
				{
					BEER_BC_SAVE_OPCODE(opcode);

					const char16* cselector = classFile->getDescriptor<StringDescriptor>(istream.read<int32>())->c_str();
					Reference<String> selector = String::gTranslate(thread, cselector);
					ostream.write((int32)selector.getId());
					
					PolymorphicCache* cache = PolymorphicCache::from(ostream.alloc(PolymorphicCache::countSize(3))); // TODO
					cache->clear(3);

					//BEER_BC_SAVE_OPCODE(BEER_INSTR_STACK_INVOKE);
				}
				break;

			case BEER_INSTR_STACK_INVOKE:
				BEER_BC_SAVE_OPCODE(opcode);
				break;

			// push int

			case BEER_INSTR_PUSH_INT8:
				{
					BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_INT64);
					
					StackRef<Integer> value(frame, frame->stackPush());
					thread->createInteger(value, istream.read<int8>()); // TODO: on permanent heap

					ostream.write<uint16>(bc->storeToPool(thread, value.staticCast<Object>()));
					frame->stackPop(); // pop value
				}
				break;

			case BEER_INSTR_PUSH_INT32:
				{
					BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_INT64);
					
					StackRef<Integer> value(frame, frame->stackPush());
					thread->createInteger(value, istream.read<int32>()); // TODO: on permanent heap

					ostream.write<uint16>(bc->storeToPool(thread, value.staticCast<Object>()));
					frame->stackPop(); // pop value
				}
				break;
			case BEER_INSTR_PUSH_INT64:
				{
					BEER_BC_SAVE_OPCODE(BEER_INSTR_PUSH_INT64);
					
					StackRef<Integer> value(frame, frame->stackPush());
					thread->createInteger(value, istream.read<int64>()); // TODO: on permanent heap

					ostream.write<uint16>(bc->storeToPool(thread, value.staticCast<Object>()));
					frame->stackPop(); // pop value
				}
				break;

			// 8 bytes = 64bit

			case BEER_INSTR_PUSH_FLOAT:
				BEER_BC_SAVE_OPCODE(opcode);
				ostream.write(istream.read<uint64>());
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}

		//ostream.write<uint32>(0);
	}

	bc->update(&ostream);
#endif // BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
}

void* Bytecode::LabelTable[BEER_MAX_OPCODE * sizeof(void*)] = {0};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_SWITCH

#define BEER_BC_PRINT() cout << *(uint8*)ip << "\t"; printTranslatedInstruction(thread, invokedMethod, reinterpret_cast<Instruction*>(ip)); cout<<"\n";

#define BEER_BC_OPCODE() *(reinterpret_cast<uint8*>(ip)++)

#define BEER_BC_DATA_PTR(type) reinterpret_cast<type>(*reinterpret_cast<int32*>(ip))

#define BEER_BC_DATA(type) *reinterpret_cast<type*>(ip)

#define BEER_BC_MOVE(howmuch) ip += howmuch

#define BEER_BC_JUMP(offset) ip += (offset - 1); frame->ip(ip);

#define BEER_BC_NEXT(skip) BEER_BC_MOVE(skip); BEER_BC_PASS();

#define BEER_BC_PASS() break;

#define BEER_BC_RETURN() frame->ip(ip); return;

#define BEER_BC_START while(true) { /*BEER_BC_PRINT();*/ switch(BEER_BC_OPCODE()) {

#define BEER_BC_END }} frame->ip(ip); return;

#define BEER_BC_LABEL(name) case BEER_##name

#define BEER_BC_DEFAULT_LABEL() default

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT

#define BEER_BC_PRINT() cout << *ip << "\t"; reinterpret_cast<Instruction*>(ip)->printTranslated(thread->getVM()); cout<<"\n";

#define BEER_BC_OPCODE() *(ip++)

#define BEER_BC_DATA_PTR(type) reinterpret_cast<type>(*reinterpret_cast<int32*>(ip))

#define BEER_BC_DATA(type) *reinterpret_cast<type*>(ip)

#define BEER_BC_MOVE(howmuch) ip += howmuch

#define BEER_BC_JUMP(instri) ip = reinterpret_cast<byte*>(getInstruction(vPC = instri));

#define BEER_BC_NEXT(skip) BEER_BC_MOVE(skip); BEER_BC_PASS();

#define BEER_BC_FETCH() /*BEER_BC_MOVE(reinterpret_cast<uint32>(ip) % 4);*/ /*ip = reinterpret_cast<byte*>(getInstruction(vPC));*/ jumpAddr = *(reinterpret_cast<void**>(ip)); BEER_BC_MOVE(sizeof(void*));

#define BEER_BC_PASS() BEER_BC_FETCH(); vPC++; __asm jmp jumpAddr;

#define BEER_BC_RETURN() frame->setProgramCounter(vPC); return;

#define BEER_BC_START ip = reinterpret_cast<byte*>(getInstruction(vPC)); BEER_BC_PASS();

#define BEER_BC_END frame->setProgramCounter(vPC); return;

#define BEER_STORE_ADDRESS(index, label)											\
{																					\
	void** data = LabelTable;														\
	__asm lea eax, label															\
	__asm mov edx, data																\
	__asm mov [edx][index * TYPE data], eax											\
}																					\

// just a shortcut for BEER_STORE_ADDRESS with one argument
#define BEER_CALLABLE_LABEL(name) BEER_STORE_ADDRESS(BEER_##name, LABEL_##name)

#define BEER_BC_LABEL(name) LABEL_##name

// TODO
#define BEER_BC_DEFAULT_LABEL() BEER_BC_LABEL(UNKNOWN_OPCODE)

#endif // BEER_BC_DISPATCH
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Bytecode::invokeBytecode(Thread* thread)
{
	Frame* frame = thread->getFrame();

	Method* invokedMethod = frame->stackTop<Method>(-1);
	DBG_ASSERT(invokedMethod != NULL, BEER_WIDEN("Method is NULL"));

	byte* ip = reinterpret_cast<byte*>(frame->ip());
	void* jumpAddr = NULL;

	if(ip == NULL)
	{
		ip = reinterpret_cast<byte*>(invokedMethod->getBytecode()->getData());
		DBG_ASSERT(ip != NULL, BEER_WIDEN("Null bytecode"));
	}


#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	if(LabelTable[BEER_FILL_OPCODE_TABLE] == NULL)
	{
		goto LABEL_FILL_OPCODE_TABLE;
	}
#endif // BEER_BC_DISPATCH

	// important for debugging
#ifdef BEER_DEBUG_MODE
	if(thread->getVM()->getDebugger()->isEnabled()) thread->getVM()->getDebugger()->step(thread, frame);
#endif // BEER_DEBUG_MODE
		
	//DBG_ASSERT(frame->programCounter < mDictSize, BEER_WIDEN("Program counter out of range"));
	//DBG_ASSERT(frame->translate(frame->stackTopIndex()) == (frame->stackSize() - 1), BEER_WIDEN("Broken stack"));

	BEER_BC_START

BEER_BC_LABEL(INSTR_NOP):
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INSTR_JMP):
	BEER_BC_JUMP(BEER_BC_DATA(int16));
	BEER_BC_NEXT(0); // zero is important!!!

BEER_BC_LABEL(INSTR_JMP_TRUE):
	{
		DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Object is NULL"));

		if(((Boolean*)frame->stackTop())->getData())
		{
			frame->stackPop();
			BEER_BC_JUMP(BEER_BC_DATA(int16));
			BEER_BC_NEXT(0); // zero is important!!!
		}

		frame->stackPop();
	}
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_JMP_FALSE):
	{
		DBG_ASSERT(frame->stackTop() != NULL, BEER_WIDEN("Object is NULL"));

		if(!((Boolean*)frame->stackTop())->getData())
		{
			frame->stackPop();
			BEER_BC_JUMP(BEER_BC_DATA(int16));
			BEER_BC_NEXT(0); // zero is important!!!
		}

		frame->stackPop();
	}
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_POP):
	frame->stackPop();
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INSTR_TOP):
	frame->stackPush(frame->stackTop(BEER_BC_DATA(int16)));
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_STORE):
	frame->stackStore(BEER_BC_DATA(int16), frame->stackTop());
	frame->stackPop();
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_MOVE_TOP):
	{
		// if/else is an optimization
		int16 move = BEER_BC_DATA(int16);
		if(move == 1)
		{
			frame->stackPush();
		}
		else if(move == 2)
		{
			frame->stackPush();
			frame->stackPush();
		}
		else
		{
			frame->stackMoveTop(move);
		}
	}
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_PUSH_INT64):
	{
		StackRef<Object> value(frame, frame->stackPush());
		invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), value);
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_PUSH_FLOAT):
	{
		StackRef<Object> value(frame, frame->stackPush());
		invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), value);
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_PUSH_STRING):
	{
		StackRef<Object> str(frame, frame->stackPush());
		invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), str);
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_PUSH_CHAR):
	frame->stackPush(Character::makeInlineValue(BEER_BC_DATA(uint8))); // TODO: char16
	BEER_BC_NEXT(sizeof(uint8));

BEER_BC_LABEL(INSTR_PUSH_BOOL):
	frame->stackPush(Boolean::makeInlineValue(BEER_BC_DATA(int8) == 1));
	BEER_BC_NEXT(sizeof(int8));

BEER_BC_LABEL(INSTR_NEW):
	{
		StackRef<Object> instance(frame, frame->stackPush());
		StackRef<Class> klass(frame, frame->stackPush());
		invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), klass);

		thread->createInstance(klass, instance);
		frame->stackPop(); // pop  klass
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_CLONE):
	throw Exception(BEER_WIDEN("Not yet implemented"), __WFILE__, __LINE__);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INSTR_ASSIGN):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		StackRef<Object> child(frame, frame->stackTopIndex() - 1);

		Object::setChild(thread, object, 2 + BEER_BC_DATA(int16), child); // TODO: find start of properties
			
		frame->stackMoveTop(-2);
	}
	BEER_BC_NEXT(sizeof(int16));

BEER_BC_LABEL(INSTR_LOAD):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		// pops object and push a child
		Object::getChild(thread, object, 2 + BEER_BC_DATA(int16), object); // TODO: find start of properties
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_LOAD_THIS):
	{
		StackRef<Object> object(frame, 0);
		NULL_ASSERT(object.get());

		StackRef<Object> child(frame, frame->stackPush());
		Object::getChild(thread, object, 2 + BEER_BC_DATA(int16), child); // TODO: find start of properties

		// *NO* pop of this !!!
	}
	BEER_BC_NEXT(sizeof(uint16));

BEER_BC_LABEL(INSTR_VIRTUAL_INVOKE):
BEER_BC_LABEL(INSTR_SPECIAL_INVOKE):
	{
		StackRef<Object> object(frame, frame->stackTopIndex());
		NULL_ASSERT(object.get());

		StackRef<Method> method(frame, frame->stackPush());

		// fetch method
		{
			StackRef<String> selector(frame, frame->stackPush());
			invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), selector);

			StackRef<Class> klass(frame, frame->stackPush());
			thread->getType(object, klass);

			Class::findMethod(thread, klass, selector, method); // lookup

			if(method.isNull())
			{
				BEER_DBG_BREAKPOINT();
				throw MethodNotFoundException(*object, *klass, *selector); // lookup failed
			}

			frame->stackMoveTop(-2); // pop selector, klass
		}

		// set new opcode
		if(true){
			Instruction* instr = reinterpret_cast<Instruction*>(ip - sizeof(OpCode));
			instr->setOpcode(BEER_OPTIMAL_CACHED_INVOKE);
			instr->setData<uint16>(invokedMethod->storeToPool(thread, method));
		}
		
		thread->openFrame();
	}
	BEER_BC_MOVE(sizeof(uint16));
	BEER_BC_RETURN();

BEER_BC_LABEL(OPTIMAL_CACHED_INVOKE):
	{
		StackRef<Method> method(frame, frame->stackPush());
		invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), method); // load from cache
		DBG_ASSERT(*method != NULL, BEER_WIDEN("Method is null"));

		thread->openFrame();
	}
	BEER_BC_MOVE(sizeof(uint16));
	BEER_BC_RETURN();
		
// no caching
BEER_BC_LABEL(INSTR_INTERFACE_INVOKE):
	{
		StackRef<Object> receiver(frame, frame->stackTopIndex());
		NULL_ASSERT(receiver.get());

		StackRef<Method> method(frame, frame->stackPush());

		// fetch method
		{
			StackRef<String> selector(frame, frame->stackPush());
			invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), selector);

			BEER_BC_MOVE(sizeof(uint16)); // pass selector

			StackRef<PolymorphicCache> cache(frame, frame->stackPush());
			invokedMethod->loadFromPool(thread, BEER_BC_DATA(uint16), cache);

			StackRef<Class> klass(frame, frame->stackPush());
			thread->getType(receiver, klass);

			PolymorphicCache::find(thread, cache, klass, selector, method);

			// lookup failed
			if(method.isNull())
			{
				throw MethodNotFoundException(*receiver, *klass, *selector);
			}

			frame->stackMoveTop(-3); // pop selector, cache, klass
		}

		thread->openFrame();
	}
	BEER_BC_MOVE(sizeof(uint16)); // pass cache
	BEER_BC_RETURN();

BEER_BC_LABEL(INSTR_STACK_INVOKE):
	thread->openFrame();
	BEER_BC_RETURN();

BEER_BC_LABEL(INSTR_RETURN):
	frame->stackMoveTop(-static_cast<int32>(BEER_BC_DATA(uint16))); // TODO
	BEER_BC_MOVE(sizeof(uint16));
	thread->closeFrame();
	BEER_BC_RETURN();

// optimalizations
#ifdef BEER_INLINE_OPTIMALIZATION
BEER_BC_LABEL(INLINE_BOOLEAN_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() == frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_NOT_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() != frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_OR):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() || frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_AND):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Boolean>(frame->stackTopIndex())->getData() && frame->stackTop<Boolean>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_BOOLEAN_NEGATION):
	frame->stackStore(frame->stackTopIndex() - 1, Boolean::makeInlineValue(
		!frame->stackTop<Boolean>(frame->stackTopIndex())->getData()
	));
	frame->stackPop();
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_PLUS):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stackTop<Integer>()->getData() + frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData();
		frame->stackMoveTop(-2);

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_MINUS):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stackTop<Integer>()->getData() - frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData();
		frame->stackMoveTop(-2);

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_MUL):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
		Integer::IntegerData result = frame->stackTop<Integer>()->getData() * frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData();
		frame->stackMoveTop(-2);

		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() == frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_NOT_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() != frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_SMALLER):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() < frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_SMALLER_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() <= frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_GREATER):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() > frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_INTEGER_GREATER_EQUAL):
	frame->stackStore(frame->stackTopIndex() - 2, Boolean::makeInlineValue(
		frame->stackTop<Integer>()->getData() >= frame->stackTop<Integer>(frame->stackTopIndex() - 1)->getData()
	));
	frame->stackMoveTop(-2);
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_GET_LENGTH):
	{
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 1);
		Integer::IntegerData result = frame->stackTop<Array>()->getSize();
		frame->stackPop();
		thread->createInteger(ret, result);
	}
	BEER_BC_NEXT(0);

/*BEER_BC_LABEL(INLINE_ARRAY_GET_ITEM):
	{
		StackRef<Array> receiver(frame, frame->stackTopIndex());
		StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
		StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);

		BOUNDS_ASSERT(index->getData(), receiver->getSize());
		ret = static_cast<Integer*>(receiver->getItem(index->getData()));

		Array::ope

		frame->stackMoveTop(-2);
	}
	BEER_BC_NEXT(0);

BEER_BC_LABEL(INLINE_ARRAY_SET_ITEM):
	{
		StackRef<Array> receiver(frame, frame->stackTopIndex());
		StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
		StackRef<Integer> value(frame, frame->stackTopIndex() - 2);

		BOUNDS_ASSERT(index->getData(), receiver->getSize());
		receiver->setItem(index->getData(), value.get());

		frame->stackMoveTop(-3);
	}
	BEER_BC_NEXT(0);*/

BEER_BC_LABEL(OPTIMAL_ARRAY_ALLOC):
	{
		StackRef<Integer> length(frame, frame->stackTopIndex());
		StackRef<Array> instance(frame, frame->stackPush());

		thread->createArray(length, instance);
	}
	BEER_BC_NEXT(0);
#endif // BEER_INLINE_OPTIMALIZATION

BEER_BC_DEFAULT_LABEL():
	throw BytecodeException(BEER_WIDEN("Unknown opcode"));
	BEER_BC_NEXT(0);

#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
LABEL_FILL_OPCODE_TABLE:
	// TODO: fill all witl LABEL_UNKNOWN_OPCODE
	// internal
	BEER_CALLABLE_LABEL(INSTR_NOP);
	BEER_CALLABLE_LABEL(INSTR_JMP);
	BEER_CALLABLE_LABEL(INSTR_JMP_TRUE);
	BEER_CALLABLE_LABEL(INSTR_JMP_FALSE);
	//BEER_CALLABLE_LABEL(INSTR_THROW);
	//BEER_CALLABLE_LABEL(INSTR_TRY_BEGIN);
	//BEER_CALLABLE_LABEL(INSTR_TRY_END);
	// stack control
	BEER_CALLABLE_LABEL(INSTR_POP);
	BEER_CALLABLE_LABEL(INSTR_TOP);
	BEER_CALLABLE_LABEL(INSTR_STORE);
	BEER_CALLABLE_LABEL(INSTR_MOVE_TOP);
	//built-in types control
	BEER_CALLABLE_LABEL(INSTR_PUSH_INT64);
	BEER_CALLABLE_LABEL(INSTR_PUSH_FLOAT);
	BEER_CALLABLE_LABEL(INSTR_PUSH_STRING);
	BEER_CALLABLE_LABEL(INSTR_PUSH_CHAR);
	BEER_CALLABLE_LABEL(INSTR_PUSH_BOOL);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_STRUCT);
	//BEER_CALLABLE_LABEL(INSTR_PUSH_FUNC);
	// object control
	BEER_CALLABLE_LABEL(INSTR_NEW);
	//BEER_CALLABLE_LABEL(INSTR_DELETE);
	BEER_CALLABLE_LABEL(INSTR_CLONE);
	BEER_CALLABLE_LABEL(INSTR_ASSIGN);
	BEER_CALLABLE_LABEL(INSTR_LOAD);
	BEER_CALLABLE_LABEL(INSTR_LOAD_THIS);
	// method control
	BEER_CALLABLE_LABEL(INSTR_VIRTUAL_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_INTERFACE_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_STATIC_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_SPECIAL_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_INVOKE);
	BEER_CALLABLE_LABEL(INSTR_RETURN);
	// Boolean
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_NOT_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_OR);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_AND);
	BEER_CALLABLE_LABEL(INLINE_BOOLEAN_NEGATION);
	// Integer
	BEER_CALLABLE_LABEL(INLINE_INTEGER_PLUS);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_MINUS);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_MUL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_NOT_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_SMALLER);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_SMALLER_EQUAL);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_GREATER);
	BEER_CALLABLE_LABEL(INLINE_INTEGER_GREATER_EQUAL);
	// Array
	BEER_CALLABLE_LABEL(INLINE_ARRAY_GET_LENGTH);
	//BEER_CALLABLE_LABEL(INLINE_ARRAY_GET_ITEM);
	//BEER_CALLABLE_LABEL(INLINE_ARRAY_SET_ITEM);
	// optimalised instructions
	BEER_CALLABLE_LABEL(OPTIMAL_INTEGER_PUSH_INLINED);
	BEER_CALLABLE_LABEL(OPTIMAL_ARRAY_ALLOC);
	// important
	BEER_CALLABLE_LABEL(FILL_OPCODE_TABLE);
#endif //BEER_BC_DISPATCH

	BEER_BC_END
}

void Bytecode::init(Thread* thread)
{
#if BEER_BC_DISPATCH == BEER_BC_DISPATCH_DIRECT
	#define DATA_SIZE 1
	byte data[DATA_SIZE];
	data[0] = BEER_FILL_OPCODE_TABLE;
	Bytecode bc(data, 1, 1);
	Frame* frame = thread->openFrame();
	frame->stackPush();
	//bc.build((VirtualMachine*)thread, NULL); // TODO
	bc.call(thread);
	frame->stackPop();
	//thread->closeFrame();
#endif //BEER_BC_DISPATCH
}