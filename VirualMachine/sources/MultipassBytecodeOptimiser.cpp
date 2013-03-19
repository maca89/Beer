#include "stdafx.h"
#include "MultipassBytecodeOptimiser.h"
#include "Bytecode.h"
#include "Thread.h"
#include "VirtualMachine.h"
#include "BytecodeOutputStream.h"
#include "BytecodeInputStream.h"

using namespace Beer;


MultipassBytecodeOptimiser::MultipassBytecodeOptimiser()
{
}

MultipassBytecodeOptimiser::~MultipassBytecodeOptimiser()
{
}

Bytecode* MultipassBytecodeOptimiser::optimise(Thread* thread, Bytecode* bc)
{
	Frame* frame = thread->getFrame();
	BEER_STACK_CHECK();

	BytecodeInputStream istream(bc->getData(), bc->getDataLength());
	BytecodeOutputStream ostream(bc->getInstructionCount());

	while(!istream.end())
	{
		ostream.next();
		Bytecode::OpCode opcode = istream.read<Bytecode::OpCode>();

		switch(opcode)
		{
			case BEER_INSTR_NOP:
			case BEER_INSTR_POP:
			case BEER_INSTR_CLONE:
			case BEER_FILL_OPCODE_TABLE:
				ostream.write<uint8>(opcode);
				break;

			case BEER_INSTR_PUSH_BOOL:
			case BEER_INSTR_PUSH_CHAR:// TODO: 16bit
				ostream.write<uint8>(opcode);
				ostream.write(istream.read<uint8>());
				break;

			case BEER_INSTR_TOP:
			case BEER_INSTR_STORE:
			case BEER_INSTR_RETURN:
			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
			case BEER_INSTR_MOVE_TOP:
			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_LOAD:
			case BEER_INSTR_ASSIGN:
			case BEER_INSTR_PUSH_STRING:
				{
					ostream.write<uint8>(opcode);
					ostream.write<int16>(istream.read<int16>());
				}
				break;

			case BEER_INSTR_NEW:
				{
					uint16 klassId = istream.read<uint16>();

					StackRef<Class> klass(frame, frame->stackPush());
					bc->loadFromPool(thread, klassId, klass);

					StackRef<Class> arrayClass(frame, frame->stackPush());
					thread->getArrayClass(arrayClass);

					if(*klass == *arrayClass)
					{
						ostream.write<uint8>(BEER_OPTIMAL_ARRAY_ALLOC);
					}
					else
					{
						ostream.write<uint8>(opcode);
						ostream.write<uint16>(klassId);
					}

					frame->stackMoveTop(-2); // pop klass, arrayClass
				}
				break;

			case BEER_INSTR_VIRTUAL_INVOKE:
			case BEER_INSTR_SPECIAL_INVOKE:
				{
					uint16 selectorId = istream.read<uint16>();

					StackRef<String> selector(frame, frame->stackPush());
					bc->loadFromPool(thread, selectorId, selector);

					Bytecode::OpCode newOpcode = thread->getVM()->getInlineFunctionTable()->find(selector);
					if(newOpcode != BEER_INSTR_NOP)
					{
						ostream.write<uint8>(newOpcode);
					}
					else
					{
						ostream.write<uint8>(opcode);
						ostream.write<uint16>(selectorId);
					}

					frame->stackMoveTop(-1); // pop selector
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
				
			case BEER_INSTR_PUSH_FLOAT:
			case BEER_INSTR_PUSH_INT64:
				{
					ostream.write<uint8>(opcode);
					ostream.write<uint16>(istream.read<uint16>());
				}
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}

	bc->update(&ostream);

	return bc;
}