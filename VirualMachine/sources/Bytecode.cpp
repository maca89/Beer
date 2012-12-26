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

using namespace Beer;

#define BEER_DEBUGGING


uint16 Bytecode::Instruction::printRaw(const ClassFileDescriptor* classFile) const
{
	uint16 size = sizeof(uint8); // opcode

	switch (opcode)
	{
	case Beer::Bytecode::INSTR_NOP:
		std::cout << "NOP";
		break;

	case Beer::Bytecode::INSTR_JMP:
		std::cout << "JMP " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_JMP_TRUE:
		std::cout << "JMP_TRUE " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_JMP_FALSE:
		std::cout << "JMP_FALSE " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_POP:
		std::cout << "POP";
		break;

	case Beer::Bytecode::INSTR_TOP:
		std::cout << "TOP " << getData_int16();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_STORE:
		std::cout << "STORE " << getData_int16();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_MOVE_TOP:
		std::cout << "MOVE_TOP " << getData_int16();
		size += sizeof(int16);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT8:
		std::cout << "PUSH_INT8 " << static_cast<uint32>(getData_int8());
		size += sizeof(int8);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT32:
		std::cout << "PUSH_INT32 " << getData_int32();
		size += sizeof(int32);
		break;

	case Beer::Bytecode::INSTR_PUSH_INT64:
		std::cout << "PUSH_INT64 " << getData_int64();
		size += sizeof(int64);
		break;

	case Beer::Bytecode::INSTR_PUSH_FLOAT:
		std::cout << "PUSH_FLOAT " << getData_float64();
		size += sizeof(float64);
		break;

	case Beer::Bytecode::INSTR_PUSH_STRING:
		std::cout << "PUSH_STRING " << getData_uint32();
		std::cout << " // \"" << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str() << "\"";
		size += sizeof(uint32);
		break;

	case Beer::Bytecode::INSTR_PUSH_CHAR:
		std::cout << "PUSH_CHAR " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_PUSH_BOOL:
		std::cout << "PUSH_BOOL " << static_cast<uint32>(getData_uint8());
		size += sizeof(uint8);
		break;

	case Beer::Bytecode::INSTR_NEW:
		std::cout << "NEW " << getData_uint32();
		std::cout << " // " << classFile->getClassName(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;

	case Beer::Bytecode::INSTR_CLONE:
		std::cout << "CLONE";
		break;

	case Beer::Bytecode::INSTR_ASSIGN:
		std::cout << "ASSIGN " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_LOAD:
		std::cout << "LOAD " << getData_uint16();
		size += sizeof(uint16);
		break;

	case Beer::Bytecode::INSTR_INVOKE:
		std::cout << "INVOKE " << getData_uint32();
		std::cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		std::cout << "INTERFACE_INVOKE " << getData_uint32();
		std::cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_STATIC_INVOKE:
		std::cout << "STATIC_INVOKE " << getData_uint32();
		std::cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_SPECIALINVOKE:
		std::cout << "SPECIALINVOKE " << getData_uint32();
		std::cout << " // " << classFile->getDescriptor<StringDescriptor>(getData_uint32())->c_str();
		size += sizeof(uint32);
		break;
	
	case Beer::Bytecode::INSTR_RETURN:
		std::cout << "RETURN " << getData_uint16();
		size += sizeof(uint16);
		break;
	
	default:
		std::cout << "[UNKNOWN]";
		break;
	}

	return size;
}

void Bytecode::Instruction::printTranslated() const
{
	switch (opcode)
	{
	case Beer::Bytecode::INSTR_NOP:
		std::cout << "NOP";
		break;

	case Beer::Bytecode::INSTR_JMP:
		std::cout << "JMP " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_JMP_TRUE:
		std::cout << "JMP_TRUE " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_JMP_FALSE:
		std::cout << "JMP_FALSE " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_POP:
		std::cout << "POP";
		break;

	case Beer::Bytecode::INSTR_TOP:
		std::cout << "TOP " << getData_int16();
		break;

	case Beer::Bytecode::INSTR_STORE:
		std::cout << "STORE " << getData_int16();
		break;

	case Beer::Bytecode::INSTR_MOVE_TOP:
		std::cout << "MOVE_TOP " << getData_int16();
		break;

	case Beer::Bytecode::INSTR_PUSH_INT8:
		std::cout << "PUSH_INT8 " << static_cast<uint32>(getData_int8());
		break;

	case Beer::Bytecode::INSTR_PUSH_INT32:
		std::cout << "PUSH_INT32 " << getData_int32();
		break;

	case Beer::Bytecode::INSTR_PUSH_INT64:
		std::cout << "PUSH_INT64 " << getData_int64();
		break;

	case Beer::Bytecode::INSTR_PUSH_FLOAT:
		std::cout << "PUSH_FLOAT " << getData_float64();
		break;

	case Beer::Bytecode::INSTR_PUSH_STRING:
		std::cout << "PUSH_STRING \"" << reinterpret_cast<const char*>(getData_uint32()) << "\"";
		break;

	case Beer::Bytecode::INSTR_PUSH_CHAR:
		std::cout << "PUSH_CHAR " << getData_uint16(); // TODO
		break;

	case Beer::Bytecode::INSTR_PUSH_BOOL:
		std::cout << "PUSH_BOOL " << static_cast<uint32>(getData_uint8());
		break;

	case Beer::Bytecode::INSTR_NEW:
		std::cout << "NEW " << reinterpret_cast<ClassReflection*>(getData_uint32())->getName();
		break;

	case Beer::Bytecode::INSTR_CLONE:
		std::cout << "CLONE";
		break;

	case Beer::Bytecode::INSTR_ASSIGN:
		std::cout << "ASSIGN " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_LOAD:
		std::cout << "LOAD " << getData_uint16();
		break;

	case Beer::Bytecode::INSTR_INVOKE:
		std::cout << "INVOKE \"" << reinterpret_cast<const char*>(getData_uint32()) << "\"";
		break;
	
	case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		std::cout << "INTERFACE_INVOKE \"" << reinterpret_cast<const char*>(getData_uint32()) << "\"";
		break;
	
	case Beer::Bytecode::INSTR_STATIC_INVOKE:
		std::cout << "STATIC_INVOKE \"" << reinterpret_cast<const char*>(getData_uint32()) << "\"";
		break;
	
	case Beer::Bytecode::INSTR_SPECIALINVOKE:
		std::cout << "SPECIAL_INVOKE \"" << reinterpret_cast<const char*>(getData_uint32()) << "\"";
		break;
	
	case Beer::Bytecode::INSTR_RETURN:
		std::cout << "RETURN " << getData_uint16();
		break;
	
	default:
		std::cout << "[UNKNOWN]";
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
				builder.copy(sizeof(uint8));
				break;

			// 2 bytes = 16bit

			case Beer::Bytecode::INSTR_RETURN:
			case Beer::Bytecode::INSTR_JMP:
			case Beer::Bytecode::INSTR_JMP_TRUE:
			case Beer::Bytecode::INSTR_JMP_FALSE:
			case Beer::Bytecode::INSTR_TOP:
			case Beer::Bytecode::INSTR_STORE:
			case Beer::Bytecode::INSTR_PUSH_CHAR:
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
				// TODO!!!
				builder.setData_int32(reinterpret_cast<int32>(classFile->getDescriptor<StringDescriptor>(builder.getData_int32())->c_str()));
				break;

			case Beer::Bytecode::INSTR_NEW:
				{
					// TODO!!!
					const char* name = classFile->getClassName(builder.getData_int32())->c_str();
					ClassReflection* klass = vm->getClass(name);
					NULL_ASSERT(klass);
					
					builder.setData_int32(reinterpret_cast<int32>(klass));
				}
				break;

			case Beer::Bytecode::INSTR_INVOKE:
				{
					// TODO!!!
					builder.setData_int32(reinterpret_cast<int32>(classFile->getDescriptor<StringDescriptor>(builder.getData_int32())->c_str()));
					
					InlineCache* cache = InlineCache::from(builder.alloc(InlineCache::countSize(mMethodCachesLength)));
					cache->clear(mMethodCachesLength);
				}
				break;
				
			// no caching
			case Beer::Bytecode::INSTR_INTERFACEINVOKE:
			case Beer::Bytecode::INSTR_STATIC_INVOKE:
			case Beer::Bytecode::INSTR_SPECIALINVOKE:
				// TODO!!!
				builder.setData_int32(reinterpret_cast<int32>(classFile->getDescriptor<StringDescriptor>(builder.getData_int32())->c_str()));
				break;

			// 8 bytes = 64bit
			case Beer::Bytecode::INSTR_PUSH_INT64:
				builder.copy(sizeof(uint64));
				break;

			case Beer::Bytecode::INSTR_PUSH_FLOAT:
				builder.copy(sizeof(uint64));
				break;

			default:
				throw BytecodeException("Unknown opcode");
				break;
		}


		//newData[bytei + (bytei - oldBytei)];
		//Instruction* newInstr = reinterpret_cast<Instruction*>(newData[bytei]);
		//newInstr->opcode = oldInstr->opcode; // copy opcode
	}

	builder.finish(&mData, mDataSize, &mDict, mDictSize);
}

void Bytecode::call(VirtualMachine* vm, StackFrame* frame)
{
	bool cont = true;
	while(cont)
	{
		// important for debugging
		if(vm->getDebugger()->isEnabled()) vm->getDebugger()->step(frame);
		
		Console::getOutput().flush(std::cout); // DBG ONLY
		
		DBG_ASSERT(frame->programCounter < mDictSize, "Program counter out of range");
		DBG_ASSERT(frame->translate(frame->stackTopIndex()) == (frame->stack->size() - 1), "Broken stack");

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

				if(cond->getData() == true)
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
			{
				/*uint16 index = instr->getData_int16();
				Object* object = frame->stackTop(index);
			
				if(object && object->getClass()->isValueType())
				{
					object = object->getClass()->cloneShallow(object, frame, vm->getHeap());
				}*/

				frame->stackPush(frame->stackTop(instr->getData_int16()));
			}
			break;

		case Beer::Bytecode::INSTR_STORE:
			{
				StackRef<Object> obj(frame, frame->stackTopIndex());
				frame->stackStore(instr->getData_int16(), obj.get());
				frame->stackPop();
			}
			break;

		case Beer::Bytecode::INSTR_MOVE_TOP:
			{
				StackRef<Object> object(frame, instr->getData_int16());
				frame->stackMoveTop(instr->getData_int16());
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_INT8:
			{
				Integer* n = vm->createInteger(static_cast<Integer::IntegerData>(instr->getData_int8()));
				frame->stackPush(n);
			}
			break;
		
		case Beer::Bytecode::INSTR_PUSH_INT32:
			{
				Integer* n = vm->createInteger(static_cast<Integer::IntegerData>(instr->getData_int32()));
				frame->stackPush(n);
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_INT64:
			{
				Integer* n = vm->createInteger(static_cast<Integer::IntegerData>(instr->getData_int64()));
				frame->stackPush(n);
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_FLOAT:
			{
				Float* n = vm->getClass("Float")->createInstance<Float>(frame, vm->getHeap());
				n->setData(instr->getData_float64());
				frame->stackPush(n);
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_STRING:
			{
				// TODO: wide string
				const char* str = reinterpret_cast<const char*>(instr->getData_int64());
				size_t size = strlen(str);
				String* s = vm->createString(size);
				s->copyData(str);
				frame->stackPush(s);
			}
			break;

		case Beer::Bytecode::INSTR_PUSH_CHAR:
			throw BytecodeException("Character is not implemented");
			break;

		case Beer::Bytecode::INSTR_PUSH_BOOL:
			{
				Boolean* b = vm->getClass("Boolean")->createInstance<Boolean>(frame, vm->getHeap());
				int8 v = instr->getData_int8();
				b->setData(v == 1);
				frame->stackPush(b);
			}
			break;

		case Beer::Bytecode::INSTR_NEW:
			{
				ClassReflection* klass = reinterpret_cast<ClassReflection*>(instr->getData_int32());
				Object* obj = klass->createInstance(frame, vm->getHeap());
				frame->stackPush(obj);
			}
			break;

		case Beer::Bytecode::INSTR_CLONE:
			{
				Object* object = frame->stackTop();
				NULL_ASSERT(object);
				object = classTable->translate(object)->cloneShallow(object, frame, vm->getHeap());
				frame->stackPush(object);
			}
			break;

		case Beer::Bytecode::INSTR_ASSIGN:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				StackRef<Object> value(frame, frame->stackTopIndex() - 1);
			
				NULL_ASSERT(object.get());
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
				StackRef<Object> object(frame, frame->stackTopIndex());
				NULL_ASSERT(object.get());

				const char* selector = reinterpret_cast<char*>(instr->getData_int32());

				// find method using inline cache
				InlineCache* cache = InlineCache::from(reinterpret_cast<byte*>(instr) + sizeof(uint8) + sizeof(int32));
				/*if(classTable->translate(object) == NULL)
				{
					classTable->translate(object);
				}*/
				/*Object* obj = object.get();
				if(obj == NULL || obj == reinterpret_cast<Object*>(0x0e))
				{
					obj = object.get();
					obj = object.get();
					std::cout << "!!!";
				}*/
				ClassReflection* klass = classTable->translate(object.get());
				MethodReflection* method = cache->find(klass, selector, mMethodCachesLength);

				// lookup failed
				if(!method)
				{
					throw MethodNotFoundException(std::string("No method ") + selector + " for " + classTable->translate(object)->getName());
				}

				// invoke method
				vm->openStackFrame(method); // vm invokes new frame
				cont = false;
			}
			break;
		
		// no caching
		case Beer::Bytecode::INSTR_INTERFACEINVOKE:
		case Beer::Bytecode::INSTR_STATIC_INVOKE:
		case Beer::Bytecode::INSTR_SPECIALINVOKE:
			{
				StackRef<Object> object(frame, frame->stackTopIndex());
				const char* selector = reinterpret_cast<char*>(instr->getData_int32());

				vm->openStackFrame(object.get(), selector); // vm invokes new frame
				cont = false;
			}
			break;

		case Beer::Bytecode::INSTR_RETURN:
			frame->stackMoveTop(-static_cast<int32>(instr->getData_uint16()));
			frame->done = true;
			cont = false;
			break;

		default:
			throw BytecodeException("Unknown opcode");
			break;
		}

		frame->programCounter = frame->nextInstruction;
	}
}