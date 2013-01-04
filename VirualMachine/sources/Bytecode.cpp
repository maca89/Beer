#include "stdafx.h"
#include "Bytecode.h"
#include "VirtualMachine.h"
#include "ObjectClass.h"
#include "StringClass.h"
#include "BooleanClass.h"
#include "IntegerClass.h"
#include "BooleanClass.h"
#include "FloatClass.h"
#include "ConsoleClass.h"
#include "MethodReflection.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "BytecodeBuilder.h"
#include "InlineCache.h"
#include "Debugger.h"
#include "ArrayClass.h"

using namespace Beer;


#define BEER_FIND_CACHED_METHOD(__out__method)																						\
	StackRef<Object> object(frame, frame->stackTopIndex());																			\
	NULL_ASSERT(object.get());																										\
	Reference<String> selector(vm->getHeap(), instr->getData_int32());																\
	InlineCache* cache = InlineCache::from(reinterpret_cast<byte*>(instr) + sizeof(uint8) + sizeof(int32));							\
	ClassReflection* klass = classTable->translate(object.get());																	\
	__out__method = cache->find(klass, selector.get(), mMethodCachesLength);														\

#ifdef BEER_MEASURE_PERFORMANCE
	#define BEER_METHOD_PERFORMANCE_START()																								\
		MethodReflection* method = NULL;																								\
		BEER_FIND_CACHED_METHOD(method);																								\
		MiliTimer timer;																												\
		timer.start();
#else
	#define BEER_METHOD_PERFORMANCE_START()
#endif // BEER_MEASURE_PERFORMANCE

#ifdef BEER_MEASURE_PERFORMANCE
	#define BEER_METHOD_PERFORMANCE_END() method->addTimeSpent(timer.stop());
#else 
	#define BEER_METHOD_PERFORMANCE_END()
#endif // BEER_MEASURE_PERFORMANCE


uint16 Bytecode::Instruction::printRaw(const ClassFileDescriptor* classFile) const
{
	uint16 size = sizeof(uint8); // opcode

	switch (opcode)
	{
	case Beer::Bytecode::INSTR_NOP:
		cout << "NOP";
		break;

	case Beer::Bytecode::INSTR_JMP:
		cout << "JMP " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_POP:
		cout << "POP";
		break;

	case Beer::Bytecode::INSTR_TOP:
		cout << "TOP " << getData_int16();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_STORE:
		cout << "STORE " << getData_int16();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData_int16();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData_int8());
		size += sizeof(int8);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData_int32();
		size += sizeof(int32);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData_int64();
		size += sizeof(int64);
		break;

	case Beer::Bytecode::INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData_float64();
		size += sizeof(float64);
		break;

	case Beer::Bytecode::INSTR_PUSH_STRING:
		cout << "PUSH_STRING " << getData_uint32();
		cout << " // \"" << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str() << "\"";
		size += sizeof(uint32);
		break;

	case Beer::Bytecode::INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16();
		//size += sizeof(uint16);
		cout << "PUSH_CHAR " << getData_uint8();
		size += sizeof(uint8);
		break;

	case Beer::Bytecode::INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData_uint8());
		size += sizeof(uint8);
		break;

	case Beer::Bytecode::INSTR_NEW:
		cout << "NEW " << getData_uint32();
		cout << " // " << classFile->getClassName(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;

	case Beer::Bytecode::INSTR_CLONE:
		cout << "CLONE";
		break;

	case Beer::Bytecode::INSTR_ASSIGN:
		cout << "ASSIGN " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_LOAD:
		cout << "LOAD " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_INVOKE:
		cout << "INVOKE " << getData_uint32();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		cout << "INTERFACE_INVOKE " << getData_uint32();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_STATIC_INVOKE:
		cout << "STATIC_INVOKE " << getData_uint32();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_SPECIALINVOKE:
		cout << "SPECIALINVOKE " << getData_uint32();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_RETURN:
		cout << "RETURN " << getData_uint16();
		size += sizeof(uint16);
		break;
	
	default:
		cout << "[UNKNOWN]";
		break;
	}

	return size;
}

void Bytecode::Instruction::printTranslated(VirtualMachine* vm) const
{
	switch (opcode)
	{
	case Beer::Bytecode::INSTR_NOP:
		cout << "NOP";
		break;

	case Beer::Bytecode::INSTR_JMP:
		cout << "JMP " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_POP:
		cout << "POP";
		break;

	case Beer::Bytecode::INSTR_TOP:
		cout << "TOP " << getData_int16();
		break;

	case Beer::Bytecode::INSTR_STORE:
		cout << "STORE " << getData_int16();
		break;

	case Beer::Bytecode::INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData_int16();
		break;

	case Beer::Bytecode::INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData_int8());
		break;

	case Beer::Bytecode::INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData_int32();
		break;

	case Beer::Bytecode::INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData_int64();
		break;

	case Beer::Bytecode::INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData_float64();
		break;

	case Beer::Bytecode::INSTR_PUSH_STRING:
		{
			Reference<String> str(vm->getHeap(), getData_uint32());
			cout << "PUSH_STRING \"" << str->c_str() << "\"";
		}
		break;

	case Beer::Bytecode::INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16(); // TODO
		cout << "PUSH_CHAR " << getData_uint8(); // TODO
		break;

	case Beer::Bytecode::INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData_uint8());
		break;

	case Beer::Bytecode::INSTR_NEW:
		cout << "NEW " << reinterpret_cast<ClassReflection*>(getData_uint32())->getName();
		break;

	case Beer::Bytecode::INSTR_CLONE:
		cout << "CLONE";
		break;

	case Beer::Bytecode::INSTR_ASSIGN:
		cout << "ASSIGN " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_LOAD:
		cout << "LOAD " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_INVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData_uint32());
			cout << "INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData_uint32());
			cout << "INTERFACE_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_STATIC_INVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData_uint32());
			cout << "STATIC_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_SPECIALINVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData_uint32());
			cout << "SPECIAL_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_RETURN:
		cout << "RETURN " << getData_uint16();
		break;
	
	default:
		cout << "[UNKNOWN]";
		break;
	}
}

void Bytecode::build(VirtualMachine* vm, ClassFileDescriptor* classFile)
{
	BytecodeBuilder builder(mData, mDataSize, mDictSize);
	
	while(!builder.done())
	{
		//Instruction* instr = builder.next();
		Bytecode::OpCode opcode = builder.next();//static_cast<Bytecode::OpCode>(instr->opcode);

		switch(opcode)
		{
			// 0 bytes = 0bit

			case Beer::Bytecode::INSTR_NOP:
			case Beer::Bytecode::INSTR_POP:
			case Beer::Bytecode::INSTR_CLONE:
				break;

			// 1 byte = 8bit

			case Beer::Bytecode::INSTR_PUSH_INT8:
			case Beer::Bytecode::INSTR_PUSH_BOOL:
			case Beer::Bytecode::INSTR_PUSH_CHAR: //
				builder.copy(sizeof(uint8));
				break;

			// 2 bytes = 16bit

			case Beer::Bytecode::INSTR_RETURN:
			case Beer::Bytecode::INSTR_JMP:
			case Beer::Bytecode::INSTR_JMP_TRUE:
			case Beer::Bytecode::INSTR_JMP_FALSE:
			case Beer::Bytecode::INSTR_TOP:
			case Beer::Bytecode::INSTR_STORE:
			//case Beer::Bytecode::INSTR_PUSH_CHAR:
			case Beer::Bytecode::INSTR_ASSIGN:
			case Beer::Bytecode::INSTR_LOAD:
			case Beer::Bytecode::INSTR_MOVE_TOP:
				builder.copy(sizeof(uint16));
				break;

			// 4 bytes = 32bit

			case Beer::Bytecode::INSTR_PUSH_INT32:
				builder.copy(sizeof(uint32));
				break;

			case Beer::Bytecode::INSTR_PUSH_STRING:
				{
					// TODO!!!
					const char16* cstring = classFile->getDescriptor<StringDescriptor>(builder.getData_int32())->c_str();
					Reference<String> string = vm->getStringClass<StringClass>()->translate(vm, cstring);

					builder.setData_int32(string.getId());
				}
				break;

			case Beer::Bytecode::INSTR_NEW:
				{
					// TODO!!!
					const char16* cstring = classFile->getClassName(builder.getData_int32())->c_str();
					const char_t* name = vm->getStringClass<StringClass>()->translate(vm, cstring)->c_str();
					ClassReflection* klass = vm->getClass(name); // TODO: pass String*
					NULL_ASSERT(klass);
					
					builder.setData_int32(reinterpret_cast<int32>(klass)); // TODO: pass Reference
				}
				break;

			case Beer::Bytecode::INSTR_INVOKE:
				{
					const char16* cselector = classFile->getDescriptor<StringDescriptor>(builder.getData_int32())->c_str();
					Reference<String> selector = vm->getStringClass<StringClass>()->translate(vm, cselector);
#ifdef BEER_INLINE_OPTIMALIZATION
					Bytecode::OpCode newOpcode = vm->getInlineFunctionTable()->find(selector.get());
					if(newOpcode != Bytecode::INSTR_NOP)
					{
						builder.setNewOpcode(newOpcode);
						//builder.setData_int32(selector.getId()); // save selecctor
					}
					//else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						// TODO!!!
						builder.setData_int32(selector.getId());
					
						InlineCache* cache = InlineCache::from(builder.alloc(InlineCache::countSize(mMethodCachesLength)));
						cache->clear(mMethodCachesLength);
					}
				}
				break;
				
			// no caching
			case Beer::Bytecode::INSTR_INTERFACEINVOKE:
			case Beer::Bytecode::INSTR_STATIC_INVOKE:
			case Beer::Bytecode::INSTR_SPECIALINVOKE:
				{
					// TODO!!!
					const char16* cselector = classFile->getDescriptor<StringDescriptor>(builder.getData_int32())->c_str();
					Reference<String> selector = vm->getStringClass<StringClass>()->translate(vm, cselector);

					builder.setData_int32(selector.getId());
				}
				break;

			// 8 bytes = 64bit
			case Beer::Bytecode::INSTR_PUSH_INT64:
				builder.copy(sizeof(uint64));
				break;

			case Beer::Bytecode::INSTR_PUSH_FLOAT:
				builder.copy(sizeof(uint64));
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}


		//newData[bytei + (bytei - oldBytei)];
		//Instruction* newInstr = reinterpret_cast<Instruction*>(newData[bytei]);
		//newInstr->opcode = oldInstr->opcode; // copy opcode
	}

	builder.finish(&mData, mDataSize, &mDict, mDictSize);
}

MethodReflection* Bytecode::call(VirtualMachine* vm, StackFrame* frame)
{
	//bool cont = true;
	while(true)
	{
		// important for debugging
	#ifdef BEER_DEBUG_MODE
		if(vm->getDebugger()->isEnabled()) vm->getDebugger()->step(frame);
	#endif // BEER_DEBUG_MODE
		
		DBG_ASSERT(frame->programCounter < mDictSize, BEER_WIDEN("Program counter out of range"));
		DBG_ASSERT(frame->translate(frame->stackTopIndex()) == (frame->stack->size() - 1), BEER_WIDEN("Broken stack"));

		ClassTable* classTable = vm->getClassTable();
		Instruction* instr = &reinterpret_cast<Instruction&>(mData[mDict[frame->programCounter]]);
		OpCode opcode = static_cast<OpCode>(instr->opcode);
		frame->nextInstruction = frame->programCounter + 1;

		switch (opcode)
		{
		case Beer::Bytecode::INSTR_NOP:
			break;

		case Beer::Bytecode::INSTR_JMP:
			frame->nextInstruction = instr->getData_uint16();
			break;

		case Beer::Bytecode::INSTR_JMP_TRUE:
			{
				StackRef<Boolean> cond(frame, frame->stackTopIndex());
				NULL_ASSERT(cond.get());

				if(cond->getData())
				{
					frame->nextInstruction = instr->getData_uint16();
				}

				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_JMP_FALSE:
			{
				StackRef<Boolean> cond(frame, frame->stackTopIndex());
				NULL_ASSERT(cond.get());

				if(cond->getData() == false)
				{
					frame->nextInstruction = instr->getData_uint16();
				}

				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_POP:
			frame->stackPop();
			break;

		case Beer::Bytecode::INSTR_TOP:
			frame->stackPush(frame->stackTop(instr->getData_int16()));
			break;

		case Beer::Bytecode::INSTR_STORE:
			{
				StackRef<Object> obj(frame, frame->stackTopIndex());
				// *NO* null checking!
				int16 index = instr->getData_int16();
				frame->stackStore(index, obj.get());
				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_MOVE_TOP:
			{
				StackRef<Object> object(frame, instr->getData_int16());
				// *NO* null checking!
				frame->stackMoveTop(instr->getData_int16());
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_INT8:
			frame->stackPush(vm->createInteger(instr->getData_int8()));
			break;
		
		case Beer::Bytecode::INSTR_PUSH_INT32:
			frame->stackPush(vm->createInteger(instr->getData_int32()));
			break;

		case Beer::Bytecode::INSTR_PUSH_INT64:
			{
				Integer* n = vm->createInteger(static_cast<Integer::IntegerData>(instr->getData_int64()));
				frame->stackPush(n);
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_FLOAT:
			{
				Float* n = vm->createFloat(instr->getData_float64());
				frame->stackPush(n);
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_STRING:
			{
				Reference<String> string(vm->getHeap(), instr->getData_int32());
				frame->stackPush(string.get());
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_CHAR:
			//throw BytecodeException(BEER_WIDEN("Character is not implemented"));
			{
				char8 c = instr->getData_uint8(); // TODO: char16
				frame->stackPush(Character::makeInlineValue(c));
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_BOOL:
			{
				int8 v = instr->getData_int8();
				frame->stackPush(Boolean::makeInlineValue(v == 1));
			}
			break;

		case Beer::Bytecode::INSTR_NEW:
			{
				ClassReflection* klass = reinterpret_cast<ClassReflection*>(instr->getData_int32());
				DBG_ASSERT(klass, BEER_WIDEN("Class is NULBEER_WIDEN("));
				Object* obj = klass->createInstance(vm, frame, vm->getHeap());
				frame->stackPush(obj);
			}
			break;

		case Beer::Bytecode::INSTR_CLONE:
			{
				Object* object = frame->stackTop();
				NULL_ASSERT(object);
				object = classTable->translate(object)->cloneShallow(vm, object, frame, vm->getHeap());
				frame->stackPush(object);
			}
			break;

		case Beer::Bytecode::INSTR_ASSIGN:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				StackRef<Object> value(frame, frame->stackTopIndex() - 1);
			
				NULL_ASSERT(object.get());
				// *NO* null assert for value!
				object->setChild(instr->getData_int16(), value.get());
			
				frame->stackPop();
				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_LOAD:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				Object* child = object->getChild<Object>(instr->getData_uint16());

				// pops object and pushed a child
				object = child;
			}
			break;

		case Beer::Bytecode::INSTR_INVOKE:
			{
				//MethodReflection* method = BEER_FIND_CACHED_METHOD();
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				Reference<String> selector(vm->getHeap(), instr->getData_int32());

				// find method using inline cache
				InlineCache* cache = InlineCache::from(reinterpret_cast<byte*>(instr) + sizeof(uint8) + sizeof(int32));
				ClassReflection* klass = classTable->translate(object.get());
				MethodReflection* method = cache->find(klass, selector.get(), mMethodCachesLength);

				// lookup failed
				if(!method)
				{
					throw MethodNotFoundException(string(BEER_WIDEN("No method ")) + selector->c_str() + BEER_WIDEN(" for ") + classTable->translate(object)->getName());
				}

				// invoke method
				frame->programCounter = frame->nextInstruction;
				return method;
			}
			break;
		
		// no caching
		case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		case Beer::Bytecode::INSTR_STATIC_INVOKE:
		case Beer::Bytecode::INSTR_SPECIALINVOKE:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				Reference<String> selector(vm->getHeap(), instr->getData_int32());
				
				ClassReflection* klass = classTable->translate(object.get());
				MethodReflection* method = klass->findMethod(selector->c_str());
				
				// lookup failed
				if(!method)
				{
					throw MethodNotFoundException(string(BEER_WIDEN("No method ")) + selector->c_str() + BEER_WIDEN(" for ") + classTable->translate(object)->getName());
				}

				// invoke method
				frame->programCounter = frame->nextInstruction; // really?
				return method;
			}
			break;

		case Beer::Bytecode::INSTR_RETURN:
			frame->stackMoveTop(-static_cast<int32>(instr->getData_uint16()));
			return NULL;
			break;

		// optimalizations
#ifdef BEER_INLINE_OPTIMALIZATION
		case Beer::Bytecode::INLINE_BOOLEAN_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Boolean> receiver(frame, frame->stackTopIndex());
				StackRef<Boolean> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				
				ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_NOT_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Boolean> receiver(frame, frame->stackTopIndex());
				StackRef<Boolean> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_OR:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Boolean> receiver(frame, frame->stackTopIndex());
				StackRef<Boolean> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() || arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_AND:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Boolean> receiver(frame, frame->stackTopIndex());
				StackRef<Boolean> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() && arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_NEGATION:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Boolean> receiver(frame, frame->stackTopIndex());
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 1);
				ret = Boolean::makeInlineValue(!receiver->getData());
				frame->stackMoveTop(-1);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_PLUS:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
				ret = vm->createInteger(receiver->getData() + arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_MINUS:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
				ret = vm->createInteger(receiver->getData() - arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_MUL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);
				ret = vm->createInteger(receiver->getData() * arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() == arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_NOT_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() != arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_SMALLER:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() < arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_SMALLER_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() <= arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_GREATER:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() > arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_GREATER_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Integer> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> arg(frame, frame->stackTopIndex() - 1);
				StackRef<Boolean> ret(frame, frame->stackTopIndex() - 2);
				ret = Boolean::makeInlineValue(receiver->getData() >= arg->getData());
				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_ARRAY_GET_LENGTH:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Array> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> ret(frame, frame->stackTopIndex() - 1);
				ret = vm->createInteger(receiver->getSize());
				frame->stackMoveTop(-1);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_ARRAY_GET_ITEM:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Array> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
				StackRef<Integer> ret(frame, frame->stackTopIndex() - 2);

				BOUNDS_ASSERT(index->getData(), receiver->getSize());
				Integer* value = static_cast<Integer*>(receiver->getItem(index->getData()));

				if(value) ret = value;
				else
				{
					// just a temporary solution to missing value types
					ret = vm->createInteger(0);
				}

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_ARRAY_SET_ITEM:
			{
				BEER_METHOD_PERFORMANCE_START();

				StackRef<Array> receiver(frame, frame->stackTopIndex());
				StackRef<Integer> index(frame, frame->stackTopIndex() - 1);
				StackRef<Integer> value(frame, frame->stackTopIndex() - 2);

				BOUNDS_ASSERT(index->getData(), receiver->getSize());
				receiver->setItem(index->getData(), value.get());

				frame->stackMoveTop(-3);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;
#endif // BEER_INLINE_OPTIMALIZATION

		default:
			throw BytecodeException(BEER_WIDEN("Unknown opcode"));
			break;
		}

		frame->programCounter = frame->nextInstruction;
	}
}