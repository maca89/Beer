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
#include "MonomorphicInlineCache.h"
#include "PolymorphicInlineCache.h"
#include "Debugger.h"
#include "ArrayClass.h"

using namespace Beer;


#define BEER_FIND_CACHED_METHOD(__out__method)																							\
	StackRef<Object> object(frame, frame->stackTopIndex());																				\
	NULL_ASSERT(object.get());																											\
	Reference<String> selector(vm->getHeap(), instr->getData_int32());																	\
	MonomorphicInlineCache* cache = MonomorphicInlineCache::from(reinterpret_cast<byte*>(instr) + sizeof(uint8) + sizeof(int32));		\
	ClassReflection* klass = vm->getClass(object.get());																				\
	__out__method = cache->find(klass, selector.get());																					\

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
		cout << "JMP " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_POP:
		cout << "POP";
		break;

	case Beer::Bytecode::INSTR_TOP:
		cout << "TOP " << getData<int16>();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_STORE:
		cout << "STORE " << getData<int16>();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData<int16>();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData<int8>());
		size += sizeof(int8);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData<int32>();
		size += sizeof(int32);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData<int64>();
		size += sizeof(int64);
		break;

	case Beer::Bytecode::INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData<float64>();
		size += sizeof(float64);
		break;

	case Beer::Bytecode::INSTR_PUSH_STRING:
		cout << "PUSH_STRING " << getData<uint32>();
		cout << " // \"" << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str() << "\"";
		size += sizeof(uint32);
		break;

	case Beer::Bytecode::INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16();
		//size += sizeof(uint16);
		cout << "PUSH_CHAR " << getData<uint8>();
		size += sizeof(uint8);
		break;

	case Beer::Bytecode::INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData<uint8>());
		size += sizeof(uint8);
		break;

	case Beer::Bytecode::INSTR_NEW:
		cout << "NEW " << getData<uint32>();
		cout << " // " << classFile->getClassName(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;

	case Beer::Bytecode::INSTR_CLONE:
		cout << "CLONE";
		break;

	case Beer::Bytecode::INSTR_ASSIGN:
		cout << "ASSIGN " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_LOAD:
		cout << "LOAD " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << getData<uint16>();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_INVOKE:
		cout << "INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		cout << "INTERFACE_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_STATIC_INVOKE:
		cout << "STATIC_INVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_SPECIALINVOKE:
		cout << "SPECIALINVOKE " << getData<uint32>();
		cout << " // " << classFile->getDescriptor<StringDescriptor>(getData<uint32>())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_RETURN:
		cout << "RETURN " << getData<uint16>();
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
		cout << "JMP " << getData<uint16>();
		break;

	case Beer::Bytecode::INSTR_JMP_TRUE:
		cout << "JMP_TRUE " << getData<uint16>();
		break;

	case Beer::Bytecode::INSTR_JMP_FALSE:
		cout << "JMP_FALSE " << getData<uint16>();
		break;

	case Beer::Bytecode::INSTR_POP:
		cout << "POP";
		break;

	case Beer::Bytecode::INSTR_TOP:
		cout << "TOP " << getData<int16>();
		break;

	case Beer::Bytecode::INSTR_STORE:
		cout << "STORE " << getData<int16>();
		break;

	case Beer::Bytecode::INSTR_MOVE_TOP:
		cout << "MOVE_TOP " << getData<int16>();
		break;

	case Beer::Bytecode::INSTR_PUSH_INT8:
		cout << "PUSH_INT8 " << static_cast<uint32>(getData<int8>());
		break;

	case Beer::Bytecode::INSTR_PUSH_INT32:
		cout << "PUSH_INT32 " << getData<int32>();
		break;

	case Beer::Bytecode::INSTR_PUSH_INT64:
		cout << "PUSH_INT64 " << getData<int64>();
		break;

	case Beer::Bytecode::INSTR_PUSH_FLOAT:
		cout << "PUSH_FLOAT " << getData<float64>();
		break;

	case Beer::Bytecode::INSTR_PUSH_STRING:
		{
			Reference<String> str(vm->getHeap(), getData<uint32>());
			cout << "PUSH_STRING \"" << str->c_str() << "\"";
		}
		break;

	case Beer::Bytecode::INSTR_PUSH_CHAR:
		//cout << "PUSH_CHAR " << getData_uint16(); // TODO
		cout << "PUSH_CHAR " << getData<uint8>(); // TODO
		break;

	case Beer::Bytecode::INSTR_PUSH_BOOL:
		cout << "PUSH_BOOL " << static_cast<uint32>(getData<uint8>());
		break;

	case Beer::Bytecode::INSTR_NEW:
		cout << "NEW " << reinterpret_cast<ClassReflection*>(getData<uint32>())->getName();
		break;

	case Beer::Bytecode::INSTR_CLONE:
		cout << "CLONE";
		break;

	case Beer::Bytecode::INSTR_ASSIGN:
		cout << "ASSIGN " << getData<uint16>();
		break;

	case Beer::Bytecode::INSTR_LOAD:
		cout << "LOAD " << getData<uint16>();
		break;

	case Beer::Bytecode::INSTR_LOAD_THIS:
		cout << "LOAD_THIS " << getData<uint16>();
		break;

	case Beer::Bytecode::INSTR_INVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "INTERFACE_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_STATIC_INVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "STATIC_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_SPECIALINVOKE:
		{
			Reference<String> selector(vm->getHeap(), getData<uint32>());
			cout << "SPECIAL_INVOKE \"" << selector->c_str() << "\"";
		}
		break;
	
	case Beer::Bytecode::INSTR_RETURN:
		cout << "RETURN " << getData<uint16>();
		break;
		
	case INLINE_BOOLEAN_EQUAL:
		cout << "INLINE_BOOLEAN_EQUAL";
		break;

	case INLINE_BOOLEAN_NOT_EQUAL:
		cout << "INLINE_BOOLEAN_NOT_EQUAL";
		break;

	case INLINE_BOOLEAN_OR:
		cout << "INLINE_BOOLEAN_OR";
		break;

	case INLINE_BOOLEAN_AND:
		cout << "INLINE_BOOLEAN_AND";
		break;

	case INLINE_BOOLEAN_NEGATION:
		cout << "INLINE_BOOLEAN_NEGATION";
		break;

	case INLINE_INTEGER_PLUS:
		cout << "INLINE_INTEGER_PLUS";
		break;

	case INLINE_INTEGER_MINUS:
		cout << "INLINE_INTEGER_MINUS";
		break;

	case INLINE_INTEGER_MUL:
		cout << "INLINE_INTEGER_MUL";
		break;

	case INLINE_INTEGER_EQUAL:
		cout << "INLINE_INTEGER_EQUAL";
		break;

	case INLINE_INTEGER_NOT_EQUAL:
		cout << "INLINE_INTEGER_NOT_EQUAL";
		break;

	case INLINE_INTEGER_SMALLER:
		cout << "INLINE_INTEGER_SMALLER";
		break;

	case INLINE_INTEGER_SMALLER_EQUAL:
		cout << "INLINE_INTEGER_SMALLER_EQUAL";
		break;

	case INLINE_INTEGER_GREATER:
		cout << "INLINE_INTEGER_GREATER";
		break;

	case INLINE_INTEGER_GREATER_EQUAL:
		cout << "INLINE_INTEGER_GREATER_EQUAL";
		break;

	case INLINE_ARRAY_GET_LENGTH:
		cout << "INLINE_ARRAY_GET_LENGTH";
		break;

	case INLINE_ARRAY_GET_ITEM:
		cout << "INLINE_ARRAY_GET_ITEM";
		break;

	case INLINE_ARRAY_SET_ITEM:
		cout << "INLINE_ARRAY_SET_ITEM";
		break;

	case OPTIMAL_INTEGER_PUSH_INLINED:
		cout << "OPTIMAL_INTEGER_PUSH_INLINED";
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
			case Beer::Bytecode::INSTR_LOAD_THIS:
			case Beer::Bytecode::INSTR_MOVE_TOP:
				builder.copy(sizeof(uint16));
				break;

			// 4 bytes = 32bit

			case Beer::Bytecode::INSTR_PUSH_INT32:
				{
#ifdef BEER_INLINE_OPTIMALIZATION
					Integer::IntegerData value = builder.getData<int32>();
					if(Integer::canBeInlineValue(value))
					{
						builder.setNewOpcode(OPTIMAL_INTEGER_PUSH_INLINED);
						builder.setData<Integer*>(Integer::makeInlineValue(value));
					}
					else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						builder.copy(sizeof(uint32));
					}
				}
				break;

			case Beer::Bytecode::INSTR_PUSH_STRING:
				{
					// TODO!!!
					const char16* cstring = classFile->getDescriptor<StringDescriptor>(builder.getData<int32>())->c_str();
					Reference<String> string = vm->getStringClass<StringClass>()->translate(vm, cstring);
					builder.setData<int32>(string.getId());
				}
				break;

			case Beer::Bytecode::INSTR_NEW:
				{
					// TODO!!!
					const char16* cstring = classFile->getClassName(builder.getData<int32>())->c_str();
					Reference<String> name = vm->getStringClass<StringClass>()->translate(vm, cstring);
					ClassReflection* klass = vm->getClass(name);
					builder.setData<ClassReflection*>(klass); // TODO: pass Reference
				}
				break;

			case Beer::Bytecode::INSTR_INVOKE:
			case Beer::Bytecode::INSTR_STATIC_INVOKE:
			case Beer::Bytecode::INSTR_SPECIALINVOKE:
				{
					const char16* cselector = classFile->getDescriptor<StringDescriptor>(builder.getData<int32>())->c_str();
					Reference<String> selector = vm->getStringClass<StringClass>()->translate(vm, cselector);
#ifdef BEER_INLINE_OPTIMALIZATION
					Bytecode::OpCode newOpcode = vm->getInlineFunctionTable()->find(selector.get());
					if(newOpcode != Bytecode::INSTR_NOP)
					{
						builder.setNewOpcode(newOpcode);
						//builder.setData_int32(selector.getId()); // save selector
					}
					//else
#endif // BEER_INLINE_OPTIMALIZATION
					{
						// TODO!!!
						builder.setData<int32>(selector.getId());
					
						MonomorphicInlineCache* cache = MonomorphicInlineCache::from(builder.alloc(MonomorphicInlineCache::countSize()));
						cache->clear();
					}
				}
				break;
				
			// no caching
			case Beer::Bytecode::INSTR_INTERFACEINVOKE:
				{
					const char16* cselector = classFile->getDescriptor<StringDescriptor>(builder.getData<int32>())->c_str();
					Reference<String> selector = vm->getStringClass<StringClass>()->translate(vm, cselector);
					builder.setData<int32>(selector.getId());
					
					PolymorphicInlineCache* cache = PolymorphicInlineCache::from(builder.alloc(PolymorphicInlineCache::countSize(mMethodCachesLength)));
					cache->clear(mMethodCachesLength);
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
	}

	builder.finish(&mData, mDataSize, &mDict, mDictSize);
}

MethodReflection* Bytecode::call(VirtualMachine* vm, StackFrame* frame)
{
	Instruction* instr = NULL;
	uint16 nextInstruction = 0;
	
	while(true)
	{
		// important for debugging
	#ifdef BEER_DEBUG_MODE
		if(vm->getDebugger()->isEnabled()) vm->getDebugger()->step(frame);
	#endif // BEER_DEBUG_MODE
		
		DBG_ASSERT(frame->programCounter < mDictSize, BEER_WIDEN("Program counter out of range"));
		DBG_ASSERT(frame->translate(frame->stackTopIndex()) == (frame->stack->size() - 1), BEER_WIDEN("Broken stack"));

		instr = getInstruction(frame->programCounter); // &reinterpret_cast<Instruction&>(mData[mDict[frame->programCounter]]);
		nextInstruction = frame->programCounter + 1;

		switch (instr->getOpcode())
		{
		case Beer::Bytecode::INSTR_NOP:
			break;

		case Beer::Bytecode::INSTR_JMP:
			nextInstruction = instr->getData<uint16>();
			break;

		case Beer::Bytecode::INSTR_JMP_TRUE:
			{
				StackRef<Boolean> cond(frame, frame->stackTopIndex());
				NULL_ASSERT(cond.get());

				if(cond->getData())
				{
					nextInstruction = instr->getData<uint16>();
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
					nextInstruction = instr->getData<uint16>();
				}

				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_POP:
			frame->stackPop();
			break;

		case Beer::Bytecode::INSTR_TOP:
			frame->stackPush(frame->stackTop(instr->getData<int16>()));
			break;

		case Beer::Bytecode::INSTR_STORE:
			{
				StackRef<Object> obj(frame, frame->stackTopIndex());
				// *NO* null checking!
				int16 index = instr->getData<int16>();
				frame->stackStore(index, obj.get());
				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_MOVE_TOP:
			frame->stackMoveTop(instr->getData<int16>());
			break;

		case Beer::Bytecode::INSTR_PUSH_INT8:
			frame->stackPush(vm->createInteger(instr->getData<int8>()));
			break;
		
		case Beer::Bytecode::INSTR_PUSH_INT32:
			frame->stackPush(vm->createInteger(instr->getData<int32>()));
			break;

		case Beer::Bytecode::INSTR_PUSH_INT64:
			frame->stackPush(vm->createInteger(static_cast<Integer::IntegerData>(instr->getData<int64>())));
			break;

		case Beer::Bytecode::INSTR_PUSH_FLOAT:
			frame->stackPush(vm->createFloat(instr->getData<float64>()));
			break;

		case Beer::Bytecode::INSTR_PUSH_STRING:
			frame->stackPush(vm->getHeap()->translate(instr->getData<int32>()));
			break;

		case Beer::Bytecode::INSTR_PUSH_CHAR:
			frame->stackPush(Character::makeInlineValue(instr->getData<uint8>())); // TODO: char16
			break;

		case Beer::Bytecode::INSTR_PUSH_BOOL:
			frame->stackPush(Boolean::makeInlineValue(instr->getData<int8>() == 1));
			break;

		case Beer::Bytecode::INSTR_NEW:
			frame->stackPush(instr->getData<ClassReflection*>()->createInstance(vm, frame, vm->getHeap())); // TODO: reference
			break;

		case Beer::Bytecode::INSTR_CLONE:
			{
				Object* object = frame->stackTop();
				NULL_ASSERT(object);
				object = vm->getClass(object)->cloneShallow(vm, object, frame, vm->getHeap());
				frame->stackPush(object);
			}
			break;

		case Beer::Bytecode::INSTR_ASSIGN:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				StackRef<Object> value(frame, frame->stackTopIndex() - 1);
			
				NULL_ASSERT(object.get());
				// *NO* null assert for value!
				object->setChild(instr->getData<int16>(), value.get());
			
				frame->stackPop();
				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_LOAD:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				Object* child = object->getChild(instr->getData<uint16>());

				// pops object and pushed a child
				object = child;
			}
			break;

		case Beer::Bytecode::INSTR_LOAD_THIS:
			{
				StackRef<Object> object(frame, 0);
				NULL_ASSERT(object.get());

				Object* child = object->getChild(instr->getData<uint16>());

				// *NO* pop of this !!!
				frame->stackPush(child);
			}
			break;

		case Beer::Bytecode::INSTR_INVOKE:
		case Beer::Bytecode::INSTR_STATIC_INVOKE:
		case Beer::Bytecode::INSTR_SPECIALINVOKE:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				Reference<String> selector(vm->getHeap(), instr->getData<int32>());

				// find method using inline cache
				MonomorphicInlineCache* cache = MonomorphicInlineCache::from(reinterpret_cast<byte*>(instr) + sizeof(uint8) + sizeof(int32));
				ClassReflection* klass = vm->getClass(object);
				MethodReflection* method = cache->find(klass, *selector);

				// lookup failed
				if(!method)
				{
					throw MethodNotFoundException(*object, klass, *selector);
				}
				
				// invoke method
				frame->programCounter = nextInstruction;
				return method;
			}
			break;
		
		// no caching
		case Beer::Bytecode::INSTR_INTERFACEINVOKE:
			{
				//MethodReflection* method = BEER_FIND_CACHED_METHOD();
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				Reference<String> selector(vm->getHeap(), instr->getData<int32>());

				// find method using inline cache
				PolymorphicInlineCache* cache = PolymorphicInlineCache::from(reinterpret_cast<byte*>(instr) + sizeof(uint8) + sizeof(int32));
				ClassReflection* klass = vm->getClass(object);
				MethodReflection* method = cache->find(klass, selector.get(), mMethodCachesLength);

				// lookup failed
				if(!method)
				{
					throw MethodNotFoundException(*object, vm->getClass(object), *selector);
				}

				// invoke method
				frame->programCounter = nextInstruction;
				return method;
			}
			break;

		case Beer::Bytecode::INSTR_RETURN:
			frame->stackMoveTop(-static_cast<int32>(instr->getData<uint16>()));
			return NULL;
			break;

		// optimalizations
#ifdef BEER_INLINE_OPTIMALIZATION
		case Beer::Bytecode::INLINE_BOOLEAN_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Boolean*>((int32)0)->getData() == frame->stack->top<Boolean*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_NOT_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Boolean*>((int32)0)->getData() != frame->stack->top<Boolean*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_OR:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Boolean*>((int32)0)->getData() || frame->stack->top<Boolean*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_AND:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Boolean*>((int32)0)->getData() && frame->stack->top<Boolean*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_BOOLEAN_NEGATION:
			{
				BEER_METHOD_PERFORMANCE_START();
				
				frame->stack->set(Boolean::makeInlineValue(
					!frame->stack->top<Boolean*>((int32)0)->getData()
				), -1);

				frame->stackMoveTop(-1);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_PLUS:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(vm->createInteger(
					frame->stack->top<Integer*>(0)->getData() + frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_MINUS:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(vm->createInteger(
					frame->stack->top<Integer*>((int32)0)->getData() - frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_MUL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(vm->createInteger(
					frame->stack->top<Integer*>((int32)0)->getData() * frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Integer*>((int32)0)->getData() == frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_NOT_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Integer*>((int32)0)->getData() != frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_SMALLER:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Integer*>((int32)0)->getData() < frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_SMALLER_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Integer*>((int32)0)->getData() <= frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_GREATER:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Integer*>((int32)0)->getData() > frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_INTEGER_GREATER_EQUAL:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(Boolean::makeInlineValue(
					frame->stack->top<Integer*>((int32)0)->getData() >= frame->stack->top<Integer*>(-1)->getData()
				), -2);

				frame->stackMoveTop(-2);

				BEER_METHOD_PERFORMANCE_END();
			}
			break;

		case Beer::Bytecode::INLINE_ARRAY_GET_LENGTH:
			{
				BEER_METHOD_PERFORMANCE_START();

				frame->stack->set(vm->createInteger(
					frame->stack->top<Array*>((int32)0)->getSize() 
				), -1);

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

		case OPTIMAL_INTEGER_PUSH_INLINED:
			frame->stackPush(reinterpret_cast<Integer*>(instr->getData<int32>()));
			break;
#endif // BEER_INLINE_OPTIMALIZATION

		default:
			throw BytecodeException(BEER_WIDEN("Unknown opcode"));
			break;
		}

		frame->programCounter = nextInstruction;
	}
}