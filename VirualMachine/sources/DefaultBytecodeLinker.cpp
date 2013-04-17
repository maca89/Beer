#include "stdafx.h"
#include "DefaultBytecodeLinker.h"
#include "Bytecode.h"
#include "VirtualMachine.h"
#include "Object.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "BytecodeInputStream.h"
#include "BytecodeOutputStream.h"
#include "PolymorphicCache.h"
#include "Method.h"

using namespace Beer;


void DefaultBytecodeLinker::link(Thread* thread, Method* method, ClassFileDescriptor* classFile, const TemporaryBytecode& bc, TemporaryBytecode& out_bc)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	BytecodeInputStream istream(bc.data, bc.dataLength);
	BytecodeOutputStream ostream(bc.instrCount);

	/*typedef std::map<int64, Integer*> IntegerCache;
	typedef std::map<int64, Integer*> IntegerCache;
	IntegerCache intCache;*/

	while(!istream.end())
	{
		ostream.next();

		Bytecode::OpCode opcode = istream.read<Bytecode::OpCode>();

		switch(opcode)
		{
			case BEER_INSTR_NOP:
			case BEER_INSTR_POP:
			case BEER_FILL_OPCODE_TABLE:
				ostream.write<uint8>(opcode);
				break;

			case BEER_INSTR_PUSH_BOOL:
				{
					ostream.write<uint8>(opcode);
					
					Boolean* value = Boolean::makeInlineValue(istream.read<uint8>() == 1);
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;

			case BEER_INSTR_PUSH_CHAR:
				{
					ostream.write<uint8>(opcode);
					
					Character* value = Character::makeInlineValue(istream.read<uint8>()); // TODO: 16bit
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;

			case BEER_INSTR_TOP:
			case BEER_INSTR_STORE:
				{
					ostream.write<uint8>(opcode);
					int16 value = istream.read<int16>();
					if(value > 0) ostream.write<int16>(value - 1); // -1 because local vars start at 0 (not at 1)
					else ostream.write<int16>(value - 2); // -2 because arguments start at -1 (not at 0) and there is method added ad -1 arg
				}
				break;

			case BEER_INSTR_RETURN:
				{
					ostream.write<uint8>(opcode);
					uint16 value = istream.read<uint16>();
					ostream.write<uint16>(0);//static_cast<uint16>(value - method->getParamsCount() - 1));
				}
				break;

			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
			case BEER_INSTR_MOVE_TOP:
			case BEER_INSTR_LOAD:
			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_ASSIGN:
			case BEER_INSTR_ASSIGN_THIS:
				ostream.write<uint8>(opcode);
				ostream.write(istream.read<uint16>());
				break;

			case BEER_INSTR_PUSH_STRING:
				{
					ostream.write<uint8>(opcode);
					// TODO: use method's pool for reusing constants

					StringDescriptor* stringDescr = classFile->getDescriptor<StringDescriptor>(istream.read<int32>());
					String* value = thread->createConstantString(stringDescr->getSize() - 1);
					value->copyData(stringDescr->c_str(), stringDescr->getSize());

					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;

			case BEER_INSTR_NEW:
				{
					const char16* cstring = classFile->getClassName(istream.read<int32>())->c_str();
					ostream.write<uint8>(opcode);

					StackRef<String> name(frame, frame->stackPush(
						*String::gTranslate(thread, cstring)
					));

					Class* klass = thread->getVM()->findClass(name);

					static Reference<String> selectorRef = String::gTranslate(thread, BEER_WIDEN("$Class::createInstance()")); // TODO;
					String* selector = *selectorRef;

					Method* method = klass->findVirtualMethod(selector);
					if(!method)
					{
						throw MethodNotFoundException(klass, klass->getSuperClass(), selector);
					}
					
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(klass))); // TODO
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(method))); // TODO

					frame->stackPop(); // pop name
				}
				break;

			case BEER_INSTR_VIRTUAL_INVOKE:
			case BEER_INSTR_SPECIAL_INVOKE:
				{
					ostream.write<uint8>(opcode);

					const char16* cselector = classFile->getDescriptor<StringDescriptor>(istream.read<int32>())->c_str();
					String* selector = *String::gTranslate(thread, cselector);

					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(selector))); // TODO
				}
				break;
				
			case BEER_INSTR_INTERFACE_INVOKE:
				{
					ostream.write<uint8>(opcode);

					const char16* cselector = classFile->getDescriptor<StringDescriptor>(istream.read<int32>())->c_str();
					StackRef<String> selector(frame, frame->stackPush(
						*String::gTranslate(thread, cselector)
					));
					ostream.write<uint16>(method->storeToPool(thread, selector));
					
					StackRef<PolymorphicCache> cache(frame, frame->stackPush());
					thread->createPolycache(cache, 3);
					ostream.write<uint16>(method->storeToPool(thread, cache));

					frame->stackMoveTop(-2); // pop selector, cache
				}
				break;

			case BEER_INSTR_STACK_INVOKE:
				ostream.write<uint8>(opcode);
				break;

			case BEER_INSTR_PUSH_INT8:
				{
					ostream.write<uint8>(BEER_INSTR_PUSH_INT64);
					// TODO: use method's pool for reusing constants
					
					Integer* value = thread->createConstantInteger(istream.read<int8>());
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;

			case BEER_INSTR_PUSH_INT32:
				{
					ostream.write<uint8>(BEER_INSTR_PUSH_INT64);
					// TODO: use method's pool for reusing constants
					
					Integer* value = thread->createConstantInteger(istream.read<int32>());
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;
			case BEER_INSTR_PUSH_INT64:
				{
					ostream.write<uint8>(BEER_INSTR_PUSH_INT64);
					// TODO: use method's pool for reusing constants
					
					Integer* value = thread->createConstantInteger(istream.read<int64>());
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;

			case BEER_INSTR_PUSH_FLOAT:
				{
					ostream.write<uint8>(opcode);
					// TODO: use method's pool for reusing constants
					
					Float* value = thread->createConstantFloat(istream.read<float64>());
					ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(value))); // TODO
				}
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}

	
	ostream.finish(&out_bc.data, out_bc.dataLength, &out_bc.dict, out_bc.instrCount);
}
