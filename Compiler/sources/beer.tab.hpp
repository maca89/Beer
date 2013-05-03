
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     KEYWORD_NATIVE = 258,
     KEYWORD_CLASS = 259,
     KEYWORD_DATA = 260,
     KEYWORD_EVENTS = 261,
     KEYWORD_INTERFACE = 262,
     KEYWORD_PROTECTED = 263,
     KEYWORD_GET = 264,
     KEYWORD_SET = 265,
     KEYWORD_NEW = 266,
     KEYWORD_THIS = 267,
     KEYWORD_RETURN = 268,
     KEYWORD_IF = 269,
     KEYWORD_ELSE_IF = 270,
     KEYWORD_ELSE = 271,
     KEYWORD_WHILE = 272,
     KEYWORD_AS = 273,
     KEYWORD_BOOLEAN = 274,
     KEYWORD_CHARACTER = 275,
     KEYWORD_INTEGER = 276,
     KEYWORD_FLOATING = 277,
     KEYWORD_STRING = 278,
     IDENTIFIER = 279,
     BOOLEAN_CONSTANT = 280,
     CHARACTER_CONSTANT = 281,
     INTEGER_CONSTANT = 282,
     FLOAT_CONSTANT = 283,
     STRING_CONSTANT = 284,
     LEFT_CURLY_BRACKET = 285,
     RIGHT_CURLY_BRACKET = 286,
     LEFT_PARENTHESIS = 287,
     RIGHT_PARENTHESIS = 288,
     SEMICOLON = 289,
     DOT = 290,
     COMMA = 291,
     ASSIGN = 292,
     PLUS_ASSIGN = 293,
     MINUS_ASSIGN = 294,
     MULTIPLICATION_ASSIGN = 295,
     DIVISION_ASSIGN = 296,
     MODULO_ASSIGN = 297,
     PLUS = 298,
     MINUS = 299,
     MULTIPLICATION = 300,
     DIVISION = 301,
     MODULO = 302,
     INCREMENT = 303,
     DECREMENT = 304,
     EQUALS = 305,
     NOT_EQUALS = 306,
     GREATER = 307,
     GREATER_OR_EQUALS = 308,
     LOWER = 309,
     LOWER_OR_EQUALS = 310,
     LOGICAL_AND = 311,
     LOGICAL_OR = 312,
     LOGICAL_NEGATION = 313,
     BITWISE_AND = 314,
     BITWISE_OR = 315,
     BITWISE_NEGATION = 316,
     BITWISE_XOR = 317,
     UNARY_MINUS = 318
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 64 "C:\\Users\\Marek\\Dropbox\\Projects\\C++\\Beer\\Compiler\\sources\\beer.y"

	// Token
	int token;

	// Value types
	bool boolean;
	char character;
	int integer;
	std::string* string;
	double floating;
		 
	// Class
	void* classDefinitions;
	void* classDefinition;

	// Section
	void* sections;
	void* section;

	// Section data
	void* memberDeclarations;
	void* memberDeclaration;
	
	// Section interface
	void* methodDefinitions;
	void* methodDefinition;
	void* returnParameters;
	void* methodParameters;
	void* methodParameter;
	
	// Statements
	void* statements;
	void* statement;

	void* block;

	void* variableDeclaration;
	void* variableDefinition;

	void* ifStatement;
	void* whileStatement;
	
	// Expressions
	void* expression;
	void* lValue;

	void* identifier;

	void* propertyCall;
	void* methodCall;
	void* constructorCall;
	void* arguments;

	// Constants 
	void* constant;



/* Line 1676 of yacc.c  */
#line 174 "C:\\Users\\Marek\\Dropbox\\Projects\\C++\\Beer\\Compiler\\sources\\beer.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


