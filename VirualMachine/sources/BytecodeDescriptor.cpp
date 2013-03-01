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
			case BEER_INSTR_NOP:
			case BEER_INSTR_POP:
			case BEER_INSTR_CLONE:
				break;

			// 1 byte = 8bit
			case BEER_INSTR_PUSH_BOOL:
			case BEER_INSTR_PUSH_INT8:
			case BEER_INSTR_PUSH_CHAR:
				// no need to convert
				bytei += sizeof(uint8);
				break;

			// 2 bytes = 16bit
			case BEER_INSTR_RETURN:
			case BEER_INSTR_JMP:
			case BEER_INSTR_JMP_TRUE:
			case BEER_INSTR_JMP_FALSE:
			case BEER_INSTR_TOP:
			case BEER_INSTR_STORE:
			//case BEER_INSTR_PUSH_CHAR:
			case BEER_INSTR_ASSIGN:
			case BEER_INSTR_LOAD:
			case BEER_INSTR_LOAD_THIS:
			case BEER_INSTR_MOVE_TOP:
				format.convert(instr->getData<uint16>());
				bytei += sizeof(uint16);
				break;

			// 4 bytes = 32bit
			case BEER_INSTR_PUSH_INT32:
				format.convert(instr->getData<uint32>());
				bytei += sizeof(uint32);
				break;

			// 4 bytes = 32bit + referenced string
			case BEER_INSTR_PUSH_STRING:
			case BEER_INSTR_INVOKE:
			case BEER_INSTR_INTERFACEINVOKE:
			case BEER_INSTR_STATIC_INVOKE:
			case BEER_INSTR_SPECIALINVOKE:
				format.convert(instr->getData<uint32>());
				bytei += sizeof(uint32);
				classFile->getDescriptor<StringDescriptor>(instr->getData<uint32>())->convert(format, classFile);
				break;

			// 8 bytes = 64bit
			case BEER_INSTR_PUSH_INT64:
			case BEER_INSTR_PUSH_FLOAT:
				format.convert(instr->getData<uint64>());
				bytei += sizeof(uint64);
				break;

			case BEER_INSTR_NEW:
				{
					format.convert(instr->getData<uint32>());
					uint32 classId = classFile->getClassId(instr->getData<uint32>());
					if(classFile->isExternalClass(classId))
					{
						classFile->getDescriptor<StringDescriptor>(classId)->convert(format, classFile);
					}
					bytei += sizeof(uint32);
				}
				break;

			default:
				throw BytecodeException(BEER_WIDEN("Unknown opcode"));
				break;
		}
	}
}