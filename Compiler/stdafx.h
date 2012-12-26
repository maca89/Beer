#pragma once

// Macros
#define INLINE

#define VERSION 1

#define FIRST_CLASS_ID 1
#define FIRST_MEMBER_ID 0
#define FIRST_FUNCTION_ID 1
#define FIRST_PARAMETER_ID 0

#ifdef _DEBUG
#define ASSERT(condition, message)										\
    do {																\
        if (! (condition)) {											\
            std::cerr << message << " in " << __FILE__					\
                      << " line " << __LINE__ << std::endl;				\
			__asm { int 3 }												\
			system("PAUSE");											\
			std::exit(EXIT_FAILURE);									\
        }																\
    } while (false)
#else
#define ASSERT(condition, message)
#endif

typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;

typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef float float32;
typedef double float64;

// STL
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <list>
#include <map>
#include <vector>

#include "Output.h"

#include "LexicalException.h"
#include "SemanticException.h"

// AST Nodes

#include "Node.h"

#include "ExpressionNode.h"
#include "ThisNode.h"
#include "ConstructorCallNode.h"
#include "ConstantNode.h"
#include "BooleanConstantNode.h"
#include "CharacterConstantNode.h"
#include "IntegerConstantNode.h"
#include "FloatConstantNode.h"
#include "StringConstantNode.h"

#include "LValueNode.h"
#include "IdentifierNode.h"

#include "StatementNode.h"

#include "BlockNode.h"

#include "AssignmentNode.h"
#include "PropertyCallNode.h"
#include "MethodCallNode.h"
#include "VariableDeclarationNode.h"
#include "IfNode.h"
#include "WhileNode.h"
#include "ReturnNode.h"

#include "MethodParameterNode.h"
#include "MethodDefinitionNode.h"

#include "MemberDeclarationNode.h"
#include "PropertyDefinitionNode.h"

#include "SectionNode.h"
#include "DataSectionNode.h"
#include "EventsSectionNode.h"
#include "InterfaceSectionNode.h"
#include "ProtectedSectionNode.h"

#include "ClassDefinitionNode.h"

#include "ProgramNode.h"


#include "VariableDefinitionNode.h"

// AST visitors
#include "NodeVisitor.h"
#include "PrinterVisitor.h"

// Result of bison
#include "beerlang.tab.hpp"

// Semantic analysis

#include "DescriptorInfo.h"

#include "StringConstant.h"

#include "MethodParameterInfo.h"

#include "VariableInfo.h"

#include "ExpressionInfo.h"

#include "BlockInfo.h"

#include "MethodInfo.h"

#include "MemberInfo.h"

#include "ClassInfo.h"

#include "DescriptorDictionary.h"

#include "ProgramInfo.h"

#include "ClassListBuilder.h"
#include "ClassStructureBuilder.h"
#include "SemanticsChecker.h"

#include "SemanticAnalyser.h"

// Code generation

#include "BinaryWriter.h"

#include "ByteCodeGenerator.h"
#include "CodeGenerator.h"
#include "ClassFileGenerator.h"


#include "BeerlangCompiler.h"