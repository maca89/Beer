#include "stdafx.h"
#include "BytecodeDescriptor.h"
#include "FileFormatConverter.h"
#include "ClassFileDescriptor.h"
#include "StringDescriptor.h"
#include "Bytecode.h"

using namespace Beer;


void BytecodeDescriptor::convert(FileFormatConverter& format, ClassFileDescriptor* classFile)
{
	// no need to convert flags
	format.convert(getInstrCount());
	format.convert(getSize());

	uint16 bytei = 0;
	for(uint16 instri = 0; instri < getInstrCount(); instri++)
	{
		Bytecode::Instruction* instr = &reinterpret_cast<Bytecode::Instruction&>(getByte(bytei));
		Bytecode::OpCode opcode = static_cast<Bytecode::OpCode>(instr->opcode);
		bytei += sizeof(uint8); // opcode
		
		switch(opcode)
		{
			// 0 bytes = 0bit
			case Beer::Bytecode::INSTR_NOP:
			case Beer::Bytecode::INSTR_POP:
			case Beer::Bytecode::INSTR_CLONE:
				break;

			// 1 byte = 8bit
			case Beer::Bytecode::INSTR_PUSH_BOOL:
			case Beer::Bytecode::INSTR_PUSH_INT8:
			case Beer::Bytecode::INSTR_PUSH_CHAR:
				// no need to convert
				bytei += sizeof(uint8);
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
				format.convert(instr->getData_uint16());
				bytei += sizeof(uint16);
				break;

			// 4 bytes = 32bit
			case Beer::Bytecode::INSTR_PUSH_INT32:
				format.convert(instr->getData_uint32());
				bytei += sizeof(uint32);
				break;

			// 4 bytes = 32bit + referenced string
			case Beer::Bytecode::INSTR_PUSH_STRING:
			case Beer::Bytecode::INSTR_NEW:
			case Beer::Bytecode::INSTR_INVOKE:
			case Beer::Bytecode::INSTR_INTERFACEINVOKE:
			case Beer::Bytecode::INSTR_STATIC_INVOKE:
			case Beer::Bytecode::INSTR_SPECIALINVOKE:
				format.convert(instr->getData_uint32());
				bytei += sizeof(uint32);
				classFile->getDescriptor<StringDescriptor>(instr->getData_uint32())->convert(format, classFile);
				break;

			// 8 bytes = 64bit
			case Beer::Bytecode::INSTR_PUSH_INT64:
			case Beer::Bytecode::INSTR_PUSH_FLOAT:
				format.convert(instr->getData_uint64());
				bytei += sizeof(uint64);
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}
}