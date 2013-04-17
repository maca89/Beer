#include "stdafx.h"
#include "MultipassBytecodeOptimiser.h"
#include "Bytecode.h"
#include "Thread.h"
#include "VirtualMachine.h"
#include "BytecodeOutputStream.h"
#include "BytecodeInputStream.h"
#include "Method.h"

using namespace Beer;


MultipassBytecodeOptimiser::MultipassBytecodeOptimiser()
{
}

MultipassBytecodeOptimiser::~MultipassBytecodeOptimiser()
{
}

void MultipassBytecodeOptimiser::optimise(Thread* thread, Method* method, const TemporaryBytecode& bc, TemporaryBytecode& out_bc)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	BytecodeInputStream istream(bc.data, bc.dataLength);
	BytecodeOutputStream ostream(bc.instrCount);

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

			case BEER_INSTR_TOP:
				{
					int16 index = istream.read<int16>();
					if(index == Frame::INDEX_RECEIVER)
					{
						ostream.write<uint8>(BEER_INSTR_PUSH_THIS);
					}
					else
					{
						ostream.write<uint8>(opcode);
						ostream.write<uint16>(index);
					}
				}
				break;

			case BEER_INSTR_RETURN:
				{
					ostream.write<uint8>(opcode);
					istream.read<uint16>(); // remove operand
				}
				break;

			case BEER_INSTR_STORE:
			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
			case BEER_INSTR_MOVE_TOP:
			case BEER_INSTR_LOAD:
			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_ASSIGN:
			case BEER_INSTR_ASSIGN_THIS:
				{
					ostream.write<uint8>(opcode);
					ostream.write<uint16>(istream.read<uint16>());
				}
				break;

			case BEER_INSTR_NEW:
				{
					Class* klass = static_cast<Class*>(reinterpret_cast<Object*>(istream.read<int32>()));
					Method* method = static_cast<Method*>(reinterpret_cast<Object*>(istream.read<int32>()));
					Class* arrayClass = thread->getVM()->getArrayClass();

					if(klass == arrayClass)
					{
						ostream.write<uint8>(BEER_OPTIMAL_ARRAY_ALLOC);
					}
					else
					{
						ostream.write<uint8>(opcode);
						ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(klass))); // TODO
						ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(method))); // TODO
					}
				}
				break;

			case BEER_INSTR_VIRTUAL_INVOKE:
			case BEER_INSTR_SPECIAL_INVOKE:
				{
					String* selector = static_cast<String*>(reinterpret_cast<Object*>(istream.read<int32>()));

					Bytecode::OpCode newOpcode = thread->getVM()->getInlineFunctionTable()->find(selector);

					// inlined method
					if(newOpcode != BEER_INSTR_NOP)
					{
						ostream.write<uint8>(newOpcode);
					}
					// virtual method
					else
					{
						String::LengthData classNameLength = Class::getFQNFromSelector(selector);
						String* className = thread->createConstantString(classNameLength);
						Class* klass = thread->getVM()->findClass(string(selector->c_str(), static_cast<size_t>(classNameLength))); // ugly, TODO: without allocating

						uint32 index = 0;
						if(!klass->findVirtualMethodIndex(selector, index))
						{
							throw MethodNotFoundException(klass, klass, selector);
						}

						if(opcode == BEER_INSTR_VIRTUAL_INVOKE)
						{
							ostream.write<uint8>(opcode);
							ostream.write<uint32>(index);
						}
						else // opcode == BEER_INSTR_SPECIAL_INVOKE
						{
							ostream.write<uint8>(opcode);
							Method* method = klass->getMethod(index);
							ostream.write<int32>(reinterpret_cast<int32>(static_cast<Object*>(method)));
						}
					}
				}
				break;
				
			case BEER_INSTR_INTERFACE_INVOKE:
				{
					ostream.write<uint8>(opcode);
					ostream.write<uint16>(istream.read<uint16>()); // selector
					ostream.write<uint16>(istream.read<uint16>()); // cache
				}
				break;

			case BEER_INSTR_STACK_INVOKE:
				ostream.write<uint8>(opcode);
				break;
				
			case BEER_INSTR_PUSH_STRING:
			case BEER_INSTR_PUSH_CHAR:
			case BEER_INSTR_PUSH_FLOAT:
			case BEER_INSTR_PUSH_INT64:
				{
					ostream.write<uint8>(opcode);
					ostream.write<int32>(istream.read<int32>());
				}
				break;

			case BEER_INSTR_PUSH_BOOL:
				{
					Boolean* value = static_cast<Boolean*>(reinterpret_cast<Object*>(istream.read<int32>()));
					
					if(value->getData() == true)
					{
						ostream.write<uint8>(BEER_OPTIMAL_PUSH_TRUE);
					}
					else
					{
						ostream.write<uint8>(BEER_OPTIMAL_PUSH_FALSE);
					}
				}
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}
	
	ostream.finish(&out_bc.data, out_bc.dataLength, &out_bc.dict, out_bc.instrCount);
}