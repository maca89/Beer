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
		class Member;
		class Method;
		class MethodParameter;

		class DescriptorDictionary;
		class StringDescriptor;
	}

	namespace CodeGeneration
	{
		typedef int8 format, compiler_flags;
		typedef int16 version, index;
		typedef uint16 length;

		class ClassFileGenerator : protected AST::NodeVisitor
		{
		public:

			enum Format
			{
				BigEndian = 1,
				LittleEndian = 2
			};

			enum Flags
			{
				Release = 1,
				Debug = 2
			};

		protected:

			std::ostream & mOut;
			std::stringstream mTemp;

			format mFormat;

			BinaryWriter mBwOut;
			BinaryWriter mBwTemp;

			class_file_address mAddr;

		public:

			ClassFileGenerator(std::ostream & out);

			void generateClassFile(SemanticAnalysis::Program * program);

		protected:

			void generateHeader(version version, compiler_flags flags);

			void generateDescriptorDictionary(SemanticAnalysis::DescriptorDictionary * dictionary);

			void generateExternalClassDescriptor(SemanticAnalysis::Class * cls);
			void generateClassDescriptor(SemanticAnalysis::Class * cls);
			void generateMemberDescriptor(SemanticAnalysis::Member * member);
			void generateMethodParameterDescriptor(SemanticAnalysis::MethodParameter * parameter);
			void generateMethodDescriptor(SemanticAnalysis::Method * method);
			void generateStringDescriptor(SemanticAnalysis::StringDescriptor * string);
			void generateByteCode(SemanticAnalysis::Method * method);

			void generateEnd();

			class_file_address getClassFileDescriptorSize(SemanticAnalysis::Program * program) const;

		};
	}
}