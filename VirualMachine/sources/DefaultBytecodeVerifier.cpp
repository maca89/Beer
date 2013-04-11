#include "stdafx.h"
#include "DefaultBytecodeVerifier.h"
#include "BytecodeInputStream.h"
#include "Thread.h"
#include "VirtualMachine.h"
#include "Bytecode.h"
#include "Class.h"
#include "Method.h"
#include "PolymorphicCache.h"

using namespace Beer;


void DefaultBytecodeVerifier::verify(Thread* thread, StackRef<Class> thisClass, StackRef<Method> method, const TemporaryBytecode& bc)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	BytecodeInputStream istream(bc.data, bc.dataLength);
	uint16 instri = 0;

	while(!istream.end())
	{
		Bytecode::OpCode opcode = istream.read<Bytecode::OpCode>();

		switch(opcode)
		{
			case BEER_INSTR_NOP:
			case BEER_INSTR_STACK_INVOKE:
				// probably nothing
				break;

			case BEER_INSTR_POP:
				// TODO: check POP if not popping too much
				break;

			case BEER_INSTR_PUSH_BOOL:
			case BEER_INSTR_PUSH_CHAR:
			case BEER_INSTR_PUSH_STRING:
			case BEER_INSTR_PUSH_FLOAT:
			case BEER_INSTR_PUSH_INT64:
				{
					Object* object = reinterpret_cast<Object*>(istream.read<int32>());

					if(object == NULL)
					{
						throw BytecodeException(BEER_WIDEN("Pushed object is NULL"));
					}
				}
				break;

			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
				{
					uint16 jmpTo = istream.read<uint16>();

					if(jmpTo >= bc.instrCount)
					{
						throw BytecodeException(BEER_WIDEN("Jump to an unspecified address"));
					}
				}
				break;

			case BEER_INSTR_TOP:
			case BEER_INSTR_STORE:
				{
					uint16 index = istream.read<uint16>();
					// TODO: check if not accessing something out of the stack
				}
				break;

			case BEER_INSTR_RETURN:
			case BEER_INSTR_MOVE_TOP:
				{
					int16 move = istream.read<int16>();
					// TODO: check if not changing stack index out bad
				}
				break;

			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_ASSIGN_THIS:
				{
					uint16 properti = istream.read<uint16>();

					if(properti >= thisClass->getPropertiesCount())
					{
						throw BytecodeException(BEER_WIDEN("Trying to access unknown property"));
					}
				}
				break;

			case BEER_INSTR_LOAD:
			case BEER_INSTR_ASSIGN:
				{
					uint16 fieldi = istream.read<uint16>();
					// TODO: check validness for known instances
				}
				break;

			case BEER_INSTR_NEW:
				{
					Class* klass = static_cast<Class*>(reinterpret_cast<Object*>(istream.read<int32>()));
					Method* method = static_cast<Method*>(reinterpret_cast<Object*>(istream.read<int32>()));

					if(klass == NULL)
					{
						throw BytecodeException(BEER_WIDEN("Class is NULL"));
					}

					if(method == NULL)
					{
						throw BytecodeException(BEER_WIDEN("Class has no method for creating instances"));
					}

					if(klass->isInterface() || klass->isAbstract())
					{
						throw BytecodeException(BEER_WIDEN("Unable to instance an abstract class or interface"));
					}
				}
				break;

			case BEER_INSTR_VIRTUAL_INVOKE:
			case BEER_INSTR_SPECIAL_INVOKE:
				{
					String* selector = static_cast<String*>(reinterpret_cast<Object*>(istream.read<int32>()));

					// TODO: what to check???

					if(selector == NULL)
					{
						throw BytecodeException(BEER_WIDEN("Selector is NULL"));
					}

					Class* selectorClass = thread->getType(selector);
					if(selectorClass != thread->getVM()->getStringClass())
					{
						throw BytecodeException(BEER_WIDEN("Selector is not a String"));
					}
				}
				break;

			case BEER_INSTR_INTERFACE_INVOKE:
				{
					StackRef<String> selector(frame, frame->stackPush());
					method->loadFromPool(thread, istream.read<uint16>(), selector);

					StackRef<PolymorphicCache> cache(frame, frame->stackPush());
					method->loadFromPool(thread, istream.read<uint16>(), cache);

					// TODO: what to check???
					
					frame->stackPop(); // pop selector
					frame->stackPop(); // pop cache
				}
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}

		instri++;
	}
}