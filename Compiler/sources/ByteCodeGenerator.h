#pragma once

#include "prereq.h"
#include "NodeVisitor.h"

#include "BinaryWriter.h"

namespace Beer
{
	namespace SemanticAnalysis
	{
		class Program;
		class Class;
		class Method;
		class Block;
	}

	namespace CodeGeneration
	{
		typedef int8 byte_code_flags;
		typedef uint8 instruction;

		class ByteCodeGenerator : public AST::NodeVisitor
		{
		public:
			enum Instructions
			{
				NOP = 0x00,
				JMP = 0x01,
				JMP_TRUE = 0x02,
				JMP_FALSE = 0x03,

				POP = 0x0A,
				TOP = 0x0B,
				STORE = 0x0C,
				MOVE_TOP = 0x0D,

				PUSH_INT8 = 0x14,

				PUSH_INT32 = 0x16,
				PUSH_INT64 = 0x17,
				PUSH_FLOAT = 0x18,
				PUSH_STRING = 0x19,
				PUSH_CHAR = 0x1A,
				PUSH_BOOL = 0x1B,

				NEW = 0x28,

				CLONE = 0x2A,
				ASSIGN = 0x2B,
				LOAD = 0x2C,

				INVOKE = 0x32,
				INTERFACE_INVOKE = 0x33,
				STATIC_INVOKE = 0x34,
				SPECIAL_INVOKE = 0x35,

				RETURN = 0x37
			};

		protected:
			BinaryWriter mBw;

			SemanticAnalysis::Program * mProgram;
			SemanticAnalysis::Class * mClass;
			SemanticAnalysis::Method * mMethod;
			SemanticAnalysis::Block * mBlock;

			count mInstructionCount;
			count mInstructionOffset;
			count mStackSize;
			count mMaxStackSize;
			class_file_address mInstructionLength;

		public:

			ByteCodeGenerator(std::ostream & out, SemanticAnalysis::Method * method, SemanticAnalysis::Block * block, bool reverseBytes = false);
			ByteCodeGenerator(std::ostream & out, SemanticAnalysis::Method * method, SemanticAnalysis::Block * block, count instructionOffset, bool reverseBytes = false);


			void generate(SemanticAnalysis::Method * method, SemanticAnalysis::Block * block);

			INLINE count getInstructionCount() const { return mInstructionCount; }
			INLINE class_file_address getInstructionLength() const { return mInstructionLength; }

			void visit(AST::BlockNode * block);

			void visit(AST::VariableDefinitionNode * variable);
			void visit(AST::AssignmentNode * assignment);
			void visit(AST::IfNode * ifNode);
			void visit(AST::WhileNode * whileNode);
			void visit(AST::ReturnNode * returnNode);

			void visit(AST::ConstructorCallNode * call);
			void visit(AST::MethodCallNode * call);

			void visit(AST::IdentifierNode * identifier);
			void visit(AST::ThisNode * thisNode);

			void visit(AST::BooleanConstantNode * constant);
			void visit(AST::CharacterConstantNode * constant);
			void visit(AST::IntegerConstantNode * constant);
			void visit(AST::FloatConstantNode * constant);
			void visit(AST::StringConstantNode * constant);

		protected:

			INLINE void changeStackSize(int16 size)
			{
				//mStackSize += size;
				//if (mStackSize > mMaxStackSize) mMaxStackSize = mStackSize;
			}

			INLINE void incStack(uint16 count = 1)
			{
				//mStackSize += count;
				//if (mStackSize > mMaxStackSize) mMaxStackSize = mStackSize;
			}

			INLINE void decStack(uint16 count = 1)
			{
				//mStackSize -= count;
			}

			INLINE void generateNOP()
			{
				mBw.write((instruction)NOP);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction);
			}

			INLINE void generateJMP(uint16 index)
			{
				mBw.write((instruction)JMP);
				mBw.write(index);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(index);
			}

			INLINE void generateJMP_FALSE(uint16 index)
			{
				mBw.write((instruction)JMP_FALSE);
				mBw.write(index);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(index);
				decStack();
			}

			INLINE void generateTOP(int16 index)
			{
				mBw.write((instruction)TOP);
				mBw.write(index);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(index);
				incStack();
			}

			INLINE void generateSTORE(int16 index)
			{
				mBw.write((instruction)STORE);
				mBw.write(index);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(index);
				decStack();
			}

			INLINE void generateMOVE_TOP(int16 count)
			{
				mBw.write((instruction)MOVE_TOP);
				mBw.write(count);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(count);
				changeStackSize(count);
			}

			INLINE void generatePUSH_BOOL(uint8 value)
			{
				mBw.write((instruction)PUSH_BOOL);
				mBw.write(value);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(value);
				incStack();
			}

			INLINE void generatePUSH_CHAR(int8 value)
			{
				mBw.write((instruction)PUSH_CHAR);
				mBw.write(value);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(value);
				incStack();
			}

			INLINE void generatePUSH_INT32(int32 value)
			{
				mBw.write((instruction)PUSH_INT32);
				mBw.write(value);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(value);			
				incStack();
			}

			INLINE void generatePUSH_FLOAT(float64 value)
			{
				mBw.write((instruction)PUSH_FLOAT);
				mBw.write(value);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(value);
				incStack();
			}

			INLINE void generatePUSH_STRING(uint32 index)
			{
				mBw.write((instruction)PUSH_STRING);
				mBw.write(index);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(index);
				incStack();
			}

			INLINE void generateNEW(uint32 id)
			{
				mBw.write((instruction)NEW);
				mBw.write(id);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(id);
				incStack();
			}

			INLINE void generateASSIGN(uint16 id)
			{
				mBw.write((instruction)ASSIGN);
				mBw.write(id);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(id);
				decStack();
			}

			INLINE void generateLOAD(uint16 id)
			{
				mBw.write((instruction)LOAD);
				mBw.write(id);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(id);
				//incStack();
			}

			INLINE void generateINVOKE(uint32 id, uint16 stackDec)
			{
				mBw.write((instruction)INVOKE);
				mBw.write(id);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(id);
				decStack(stackDec);
			}

			INLINE void generateINTERFACE_INVOKE(uint32 id, uint16 stackDec)
			{
				mBw.write((instruction)INTERFACE_INVOKE);
				mBw.write(id);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(id);
				decStack(stackDec);
			}

			INLINE void generateSPECIAL_INVOKE(uint32 id, uint16 stackDec)
			{
				mBw.write((instruction)SPECIAL_INVOKE);
				mBw.write(id);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(id);
				decStack(stackDec);
			}

			INLINE void generateRETURN(uint16 count, uint16 stackDec)
			{
				mBw.write((instruction)RETURN);
				mBw.write(count);
				mInstructionCount++;
				mInstructionLength += sizeof(instruction) + sizeof(count);
				decStack(stackDec);
			}
		};
	}
}