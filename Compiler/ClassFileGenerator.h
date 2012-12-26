#include "stdafx.h"

namespace BeerlangCompiler
{
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

			class_file_address mCurrentAddress;

		public:

			ClassFileGenerator(std::ostream & out);

			void generateClassFile(SemanticAnalysis::ProgramInfo * program);

		protected:

			void generateHeader(version version, compiler_flags flags);

			void generateDescriptorDictionary(SemanticAnalysis::DescriptorDictionary * dictionary);

			void generateExternalClassDescriptor(SemanticAnalysis::ClassInfo * cls);
			void generateClassDescriptor(SemanticAnalysis::ClassInfo * cls);
			void generateMemberDescriptor(SemanticAnalysis::MemberInfo * member);
			void generateMethodParameterDescriptor(SemanticAnalysis::MethodParameterInfo * parameter);
			void generateMethodDescriptor(SemanticAnalysis::MethodInfo * method);
			void generateStringDescriptor(SemanticAnalysis::StringConstant * string);
			void generateByteCode(SemanticAnalysis::MethodInfo * method);

			void generateEnd();

			class_file_address getClassFileDescriptorSize(SemanticAnalysis::ProgramInfo * program) const;


		};
	}
}