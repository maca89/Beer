#pragma once

namespace Beer
{
	namespace AST
	{
		class ExpressionNode;
		class MethodParameterNode;
	
		typedef std::list<ExpressionNode *> ExpressionList;
		typedef std::list<MethodParameterNode *> MethodParameterList;
	}

	namespace SemanticAnalysis
	{
		class Block;
		class MethodParameter;

		typedef std::list<MethodParameter *> MethodParameterList;

		class NameBuilder
		{
		public:

			static std::string buildFullName(const std::string & className, const std::string & methodName, AST::MethodParameterList * parameters);
			static std::string buildFullName(const std::string & className, const std::string & methodName, MethodParameterList * parameters);
			static std::string buildFullName(const std::string & className, const std::string & methodName, AST::ExpressionList * arguments, Block * block);

			static std::string buildQualifiedName(const std::string & methodName, AST::MethodParameterList * parameters);
			static std::string buildQualifiedName(const std::string & methodName, MethodParameterList * parameters);
			static std::string buildQualifiedName(const std::string & methodName, AST::ExpressionList * arguments, Block * block);
		};
	}
}