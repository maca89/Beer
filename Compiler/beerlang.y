%{
#pragma once

#include "stdafx.h"

using namespace BeerlangCompiler::AST;

extern size_t lineNumber;
extern std::string filename;
extern BeerlangCompiler::Output * out;
ClassDefinitionList * bClasses;

int yylex(); 
int yyerror(const char *p)
{
	std::stringstream ss;
	ss << (lineNumber - 1);
	std::string ln;
	ss >> ln;
	ss.clear();
	out->addError(std::string(p) + " in file " + filename + " around line number " + ln);
	return 0;
}


%}

%error-verbose

//-- SYMBOL SEMANTIC VALUES -----------------------------
%union {
	// Token
	int token;

	// Value types
	bool boolean;
	char character;
	int integer;
	std::string * string;
	double floating;
		 
	// Class
	BeerlangCompiler::AST::ClassDefinitionList * classDefinitions;
	BeerlangCompiler::AST::ClassDefinitionNode * classDefinition;

	// Section
	BeerlangCompiler::AST::Sections * sections;
	BeerlangCompiler::AST::SectionNode * section;

	// Section data
	BeerlangCompiler::AST::MemberDeclarationList * memberDeclarations;
	BeerlangCompiler::AST::MemberDeclarationNode * memberDeclaration;
	
	// Section interface

	BeerlangCompiler::AST::MethodDefinitionList * methodDefinitions;
	BeerlangCompiler::AST::MethodDefinitionNode * methodDefinition;
	BeerlangCompiler::AST::StringList * returnParameters;
	BeerlangCompiler::AST::MethodParameterList * methodParameters;
	BeerlangCompiler::AST::MethodParameterNode * methodParameter;
	
		
	// Statements

	BeerlangCompiler::AST::Statements * statements;
	BeerlangCompiler::AST::StatementNode * statement;

	BeerlangCompiler::AST::BlockNode * block;

	BeerlangCompiler::AST::VariableDeclarationNode * variableDeclaration;
	BeerlangCompiler::AST::VariableDefinitionNode * variableDefinition;

	BeerlangCompiler::AST::IfNode * ifStatement;
	BeerlangCompiler::AST::WhileNode * whileStatement;
	
	// Expressions
	BeerlangCompiler::AST::ExpressionNode * expression;
	BeerlangCompiler::AST::LValueNode * lValue;

	BeerlangCompiler::AST::IdentifierNode * identifier;

	BeerlangCompiler::AST::PropertyCallNode * propertyCall;
	BeerlangCompiler::AST::MethodCallNode * methodCall;
	BeerlangCompiler::AST::ConstructorCallNode * constructorCall;
	BeerlangCompiler::AST::ExpressionList * arguments;

	// Constants 
	BeerlangCompiler::AST::ConstantNode * constant;
};

// Tokens
%token KEYWORD_NATIVE KEYWORD_CLASS KEYWORD_DATA KEYWORD_EVENTS KEYWORD_INTERFACE KEYWORD_PROTECTED KEYWORD_GET KEYWORD_SET KEYWORD_NEW KEYWORD_THIS KEYWORD_RETURN KEYWORD_IF KEYWORD_ELSE_IF KEYWORD_ELSE KEYWORD_WHILE KEYWORD_AS

%token <string> KEYWORD_BOOLEAN KEYWORD_CHARACTER KEYWORD_INTEGER KEYWORD_FLOATING KEYWORD_STRING IDENTIFIER

%token <boolean> BOOLEAN_CONSTANT
%token <character> CHARACTER_CONSTANT
%token <integer> INTEGER_CONSTANT
%token <floating> FLOAT_CONSTANT
%token <string> STRING_CONSTANT

%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON DOT COMMA 

// operators
%token <token> ASSIGN PLUS_ASSIGN MINUS_ASSIGN MULTIPLICATION_ASSIGN DIVISION_ASSIGN MODULO_ASSIGN
%token <string> PLUS MINUS MULTIPLICATION DIVISION MODULO
%token <string> INCREMENT DECREMENT
%token <string> EQUALS NOT_EQUALS GREATER GREATER_OR_EQUALS LOWER LOWER_OR_EQUALS
%token <string> LOGICAL_AND LOGICAL_OR LOGICAL_NEGATION
%token <string> BITWISE_AND BITWISE_OR BITWISE_NEGATION BITWISE_XOR

// Types

%type <classDefinitions> classDefinitions
%type <classDefinition> classDefinition

%type <sections> sections
%type <section> section
%type <section> dataSection
%type <section> eventsSection
%type <section> interfaceSection
%type <section> protectedSection

%type <memberDeclarations> memberDeclarations
%type <memberDeclaration> memberDeclaration

%type <variableDeclaration> variableDeclaration

%type <returnParameters> returnParameters
%type <methodParameters> methodParameters
%type <methodParameter> methodParameter
%type <methodDefinitions> methodDefinitions
%type <methodDefinition> methodDefinition

%type <methodDefinition> unaryOperatorDefinition
%type <methodDefinition> binaryOperatorDefinition

%type <block> block
%type <statements> statements
%type <statement> statement

%type <statement> assign
%type <ifStatement> if
%type <whileStatement> while
%type <statement> return

%type <expression> expression

%type <lValue> lValue

%type <identifier> identifier

%type <constructorCall> constructorCall
%type <methodCall> methodCall
%type <methodCall> unaryOperatorCall
%type <methodCall> binaryOperatorCall
%type <propertyCall> propertyCall

%type <arguments> arguments
%type <expression> argument

%type <constant> constant
%type <constant> booleanConstant
%type <constant> characterConstant
%type <constant> integerConstant
%type <constant> floatConstant
%type <constant> stringConstant

%type <variableDefinition> variableDefinition

%type <string> prefixUnaryOperator
%type <string> postfixUnaryOperator
%type <string> unaryOperator
%type <string> binaryOperator

%left IDENTIFIER
%left ASSIGN
%left KEYWORD_AS
%left LOGICAL_AND LOGICAL_OR
%left BITWISE_AND BITWISE_OR BITWISE_XOR
%left EQUALS NOT_EQUALS
%left GREATER GREATER_OR_EQUALS LOWER LOWER_OR_EQUALS
%left PLUS MINUS
%left MULTIPLICATION DIVISION MODULO
%left INCREMENT DECREMENT BITWISE_NEGATION LOGICAL_NEGATION UNARY_MINUS
%left DOT

//-- GRAMMAR RULES ---------------------------------------
%%

classDefinitions:
	{ bClasses = new ClassDefinitionList; } |
	classDefinition { bClasses = new ClassDefinitionList; bClasses->push_back($1); } |
	classDefinitions classDefinition { bClasses->push_back($2); } 

classDefinition:
	KEYWORD_NATIVE KEYWORD_CLASS IDENTIFIER LEFT_CURLY_BRACKET sections RIGHT_CURLY_BRACKET { $$ = new ClassDefinitionNode(true, *$3, $5); } |
	KEYWORD_CLASS IDENTIFIER LEFT_CURLY_BRACKET sections RIGHT_CURLY_BRACKET { $$ = new ClassDefinitionNode(*$2, $4); }
	
sections:
	{ $$ = new Sections; } |
	section { $$ = new Sections; $$->push_back($1); } |
	sections section { $$ = $1; $$->push_back($2); }

section:
	dataSection { $$ = $1 } |
	eventsSection { $$ = $1 } |
	interfaceSection { $$ = $1 } |
	protectedSection { $$ = $1 }

dataSection:
	KEYWORD_DATA LEFT_CURLY_BRACKET memberDeclarations RIGHT_CURLY_BRACKET { $$ = new DataSectionNode($3); } 

memberDeclarations:
	{ $$ = new MemberDeclarationList; } |
	memberDeclaration { $$ = new MemberDeclarationList; $$->push_back($1); } |
	memberDeclarations memberDeclaration { $$ = $1; $$->push_back($2); }

memberDeclaration:
	IDENTIFIER IDENTIFIER SEMICOLON { $$ = new MemberDeclarationNode(*$1, *$2); }

eventsSection: KEYWORD_EVENTS LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET { $$ = new EventsSectionNode; }

interfaceSection:
	KEYWORD_INTERFACE LEFT_CURLY_BRACKET methodDefinitions RIGHT_CURLY_BRACKET { $$ = new InterfaceSectionNode($3); } |
	KEYWORD_INTERFACE IDENTIFIER LEFT_CURLY_BRACKET methodDefinitions RIGHT_CURLY_BRACKET { $$ = new InterfaceSectionNode(*$2, $4); }

protectedSection: KEYWORD_PROTECTED LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET { $$ = new ProtectedSectionNode; }

variableDeclaration:
	IDENTIFIER IDENTIFIER { $$ = new VariableDeclarationNode(*$1, *$2); }
	

returnParameters:
	IDENTIFIER { $$ = new StringList; $$->push_back(*$1); } |
	returnParameters COMMA IDENTIFIER { $$ = $1; $$->push_back(*$3); }

methodParameters:
	{ $$ = new MethodParameterList; } |
	methodParameter { $$ = new MethodParameterList; $$->push_back($1); } |
	methodParameters COMMA methodParameter { $$ = $1; $$->push_back($3); }
	
methodParameter:
	IDENTIFIER IDENTIFIER { $$ = new MethodParameterNode(*$1, *$2); } |
	IDENTIFIER IDENTIFIER ASSIGN constant { $$ = new MethodParameterNode(*$1, *$2, $4); }

methodDefinitions:
	{ $$ = new MethodDefinitionList; } |
	methodDefinition { $$ = new MethodDefinitionList; $$->push_back($1); } |
	methodDefinitions methodDefinition { $$ = $1; $$->push_back($2); }

methodDefinition:
	IDENTIFIER SEMICOLON { $$ = new MethodDefinitionNode(*$1); } |
	IDENTIFIER block { $$ = new MethodDefinitionNode(*$1, $2); } |
	IDENTIFIER LEFT_PARENTHESIS methodParameters RIGHT_PARENTHESIS SEMICOLON { $$ = new MethodDefinitionNode(*$1, $3); } |
	IDENTIFIER LEFT_PARENTHESIS methodParameters RIGHT_PARENTHESIS block { $$ = new MethodDefinitionNode(*$1, $3, $5); } |
	returnParameters IDENTIFIER SEMICOLON { $$ = new MethodDefinitionNode($1, *$2); } |
	returnParameters IDENTIFIER block { $$ = new MethodDefinitionNode($1, *$2, $3); } |
	returnParameters IDENTIFIER LEFT_PARENTHESIS methodParameters RIGHT_PARENTHESIS SEMICOLON { $$ = new MethodDefinitionNode($1, *$2, $4); } |
	returnParameters IDENTIFIER LEFT_PARENTHESIS methodParameters RIGHT_PARENTHESIS block { $$ = new MethodDefinitionNode($1, *$2, $4, $6); } |
	IDENTIFIER MINUS LEFT_PARENTHESIS methodParameter RIGHT_PARENTHESIS SEMICOLON { $$ = new MethodDefinitionNode(*$1, *$2, $4, NULL); } | // because of binary minus
	IDENTIFIER MINUS LEFT_PARENTHESIS methodParameter RIGHT_PARENTHESIS block { $$ = new MethodDefinitionNode(*$1, *$2, $4, $6); } | // because of binary minus
	IDENTIFIER MINUS SEMICOLON { $$ = new MethodDefinitionNode(*$1, *$2); } | // because of unary minus
	IDENTIFIER MINUS block { $$ = new MethodDefinitionNode(*$1, *$2, $3); } | // because of unary minus
	IDENTIFIER MINUS LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON { $$ = new MethodDefinitionNode(*$1, *$2); } | // because of unary minus
	IDENTIFIER MINUS LEFT_PARENTHESIS RIGHT_PARENTHESIS block { $$ = new MethodDefinitionNode(*$1, *$2, $5); } | // because of unary minus
	binaryOperatorDefinition { $$ = $1; } |
	unaryOperatorDefinition { $$ = $1; }
	

unaryOperatorDefinition:
	IDENTIFIER unaryOperator SEMICOLON { $$ = new MethodDefinitionNode(*$1, *$2); } |
	IDENTIFIER unaryOperator block { $$ = new MethodDefinitionNode(*$1, *$2, $3); } |
	IDENTIFIER unaryOperator LEFT_PARENTHESIS RIGHT_PARENTHESIS SEMICOLON { $$ = new MethodDefinitionNode(*$1, *$2); } |
	IDENTIFIER unaryOperator LEFT_PARENTHESIS RIGHT_PARENTHESIS block { $$ = new MethodDefinitionNode(*$1, *$2, $5); }

binaryOperatorDefinition:
	IDENTIFIER binaryOperator LEFT_PARENTHESIS methodParameter RIGHT_PARENTHESIS SEMICOLON { $$ = new MethodDefinitionNode(*$1, *$2, $4, NULL); } |
	IDENTIFIER binaryOperator LEFT_PARENTHESIS methodParameter RIGHT_PARENTHESIS block { $$ = new MethodDefinitionNode(*$1, *$2, $4, $6); }


block:
	LEFT_CURLY_BRACKET statements RIGHT_CURLY_BRACKET {$$ = new BlockNode($2); }

statements:
	{ $$ = new Statements; } |
	statement { $$ = new Statements; $$->push_back($1); } |
	statements statement { $$->push_back($2); }

statement:
	variableDefinition SEMICOLON { $$ = $1; } |
	variableDeclaration SEMICOLON { $$ = $1; } |
	assign SEMICOLON { $$ = $1; } |
	if { $$ = $1; } |
	while { $$ = $1; } |
	return SEMICOLON { $$ = $1; } |
	methodCall SEMICOLON { $$ = $1; $1->setType(MethodCallNode::Statement); }
	

variableDefinition:
	variableDeclaration ASSIGN expression { $$ = new VariableDefinitionNode($1, $3); }
	
assign:
	lValue ASSIGN expression { $$ = new AssignmentNode($1, $3); } |
	lValue PLUS_ASSIGN expression { $$ = new AssignmentNode($1, new MethodCallNode($1, "+", $3)); } |
	lValue MINUS_ASSIGN expression { $$ = new AssignmentNode($1, new MethodCallNode($1, "-", $3)); } |
	lValue MULTIPLICATION_ASSIGN expression { $$ = new AssignmentNode($1, new MethodCallNode($1, "*", $3)); } |
	lValue DIVISION_ASSIGN expression { $$ = new AssignmentNode($1, new MethodCallNode($1, "/", $3)); } |
	lValue MODULO_ASSIGN expression { $$ = new AssignmentNode($1, new MethodCallNode($1, "%", $3)); }

if:
	KEYWORD_IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS block { $$ = new IfNode($3, $5); } |
	if KEYWORD_ELSE_IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS block { $$ = $1; $$->addIfBlock($4, $6); } |
	if KEYWORD_ELSE block { $$ = $1; $$->addElseBlock($3); }

while:
	KEYWORD_WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS block { $$ = new WhileNode($3, $5); }

return:
	KEYWORD_RETURN arguments { $$ = new ReturnNode($2); }

expression:
	lValue { $$ = $1; } |
	constant { $$ = $1; } |
	constructorCall { $$ = $1; } |
	methodCall { $$ = $1; } |
	propertyCall { $$ = $1; } |
	KEYWORD_THIS { $$ = new ThisNode; } |
	LEFT_PARENTHESIS expression RIGHT_PARENTHESIS { $$ = $2; }

lValue:
	identifier { $$ = $1; }

identifier:
	IDENTIFIER { $$ = new IdentifierNode(*$1); }

constant:
	booleanConstant { $$ = $1; } |
	characterConstant { $$ = $1; } |
	integerConstant { $$ = $1; } |
	floatConstant { $$ = $1; } |
	stringConstant { $$ = $1; }

booleanConstant:
	BOOLEAN_CONSTANT { $$ = new BooleanConstantNode($1); }

characterConstant:
	CHARACTER_CONSTANT { $$ = new CharacterConstantNode($1); }

integerConstant:
	INTEGER_CONSTANT { $$ = new IntegerConstantNode($1); }

floatConstant:
	FLOAT_CONSTANT { $$ = new FloatConstantNode($1); }

stringConstant:
	STRING_CONSTANT { $$ = new StringConstantNode(*$1); }

constructorCall:
	KEYWORD_NEW IDENTIFIER LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS { $$ = new ConstructorCallNode(*$2, $4); }
	
methodCall:
	KEYWORD_THIS DOT IDENTIFIER LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS { $$ = new MethodCallNode(*$3, $5); } |
	IDENTIFIER LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS { $$ = new MethodCallNode(*$1, $3); } |
	expression DOT IDENTIFIER LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS { $$ = new MethodCallNode($1, *$3, $5); } |
	expression KEYWORD_AS IDENTIFIER { $$ = new MethodCallNode($1, *$3); } |
	unaryOperatorCall { $$ = $1 } |
	binaryOperatorCall { $$ = $1 }

unaryOperatorCall:
	MINUS expression %prec UNARY_MINUS { $$ = new MethodCallNode($2, *$1); } |

	expression INCREMENT { $$ = new MethodCallNode($1, *$2); } |
	expression DECREMENT { $$ = new MethodCallNode($1, *$2); } |
	
	BITWISE_NEGATION expression { $$ = new MethodCallNode($2, *$1); } |
	LOGICAL_NEGATION expression { $$ = new MethodCallNode($2, *$1); }
	
binaryOperatorCall:
	expression PLUS expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression MINUS expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression MULTIPLICATION expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression DIVISION expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression MODULO expression { $$ = new MethodCallNode($1, *$2, $3); } |	

	expression EQUALS expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression NOT_EQUALS expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression GREATER expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression GREATER_OR_EQUALS expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression LOWER expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression LOWER_OR_EQUALS expression { $$ = new MethodCallNode($1, *$2, $3); } |
	
	expression LOGICAL_AND expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression LOGICAL_OR expression { $$ = new MethodCallNode($1, *$2, $3); } |

	expression BITWISE_AND expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression BITWISE_OR expression { $$ = new MethodCallNode($1, *$2, $3); } |
	expression BITWISE_XOR expression { $$ = new MethodCallNode($1, *$2, $3); }	  
	     

propertyCall:
	expression DOT IDENTIFIER { $$ = new PropertyCallNode($1, *$3); }

arguments:
	{ $$ = new ExpressionList; } |
	argument { $$ = new ExpressionList; $$->push_back($1); } |
	arguments COMMA argument { $$ = $1; $$->push_back($3); }

argument:
	expression { $$ = $1; }

prefixUnaryOperator:
	MINUS { $$ = $1; } |
	LOGICAL_NEGATION { $$ = $1; } |
	BITWISE_NEGATION { $$ = $1; }

postfixUnaryOperator:
	INCREMENT { $$ = $1; } |
	DECREMENT { $$ = $1; }

unaryOperator:
	prefixUnaryOperator { $$ = $1; } |
	postfixUnaryOperator { $$ = $1; }

binaryOperator:
	PLUS { $$ = $1; } |
	MINUS { $$ = $1; } |
	MULTIPLICATION { $$ = $1; } |
	DIVISION { $$ = $1; } |
	MODULO { $$ = $1; } |
	EQUALS { $$ = $1; } |
	NOT_EQUALS { $$ = $1; } |
	GREATER { $$ = $1; } |
	GREATER_OR_EQUALS { $$ = $1; } |
	LOWER { $$ = $1; } |
	LOWER_OR_EQUALS { $$ = $1; } |
	LOGICAL_AND { $$ = $1; } |
	LOGICAL_OR { $$ = $1; } |
	BITWISE_AND { $$ = $1; } |
	BITWISE_OR { $$ = $1; } |
	BITWISE_XOR { $$ = $1; }

%%