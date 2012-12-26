
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "beerlang.y"

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




/* Line 189 of yacc.c  */
#line 101 "beerlang.tab.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 214 of yacc.c  */
#line 31 "beerlang.y"

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



/* Line 214 of yacc.c  */
#line 262 "beerlang.tab.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 274 "beerlang.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   464

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  64
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  153
/* YYNRULES -- Number of states.  */
#define YYNSTATES  261

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     9,    16,    22,    23,    25,
      28,    30,    32,    34,    36,    41,    42,    44,    47,    51,
      55,    60,    66,    70,    73,    75,    79,    80,    82,    86,
      89,    94,    95,    97,   100,   103,   106,   112,   118,   122,
     126,   133,   140,   147,   154,   158,   162,   168,   174,   176,
     178,   182,   186,   192,   198,   205,   212,   216,   217,   219,
     222,   225,   228,   231,   233,   235,   238,   241,   245,   249,
     253,   257,   261,   265,   269,   275,   282,   286,   292,   295,
     297,   299,   301,   303,   305,   307,   311,   313,   315,   317,
     319,   321,   323,   325,   327,   329,   331,   333,   335,   341,
     348,   353,   360,   364,   366,   368,   371,   374,   377,   380,
     383,   387,   391,   395,   399,   403,   407,   411,   415,   419,
     423,   427,   431,   435,   439,   443,   447,   451,   452,   454,
     458,   460,   462,   464,   466,   468,   470,   472,   474,   476,
     478,   480,   482,   484,   486,   488,   490,   492,   494,   496,
     498,   500,   502,   504
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      65,     0,    -1,    -1,    66,    -1,    65,    66,    -1,     3,
       4,    24,    30,    67,    31,    -1,     4,    24,    30,    67,
      31,    -1,    -1,    68,    -1,    67,    68,    -1,    69,    -1,
      72,    -1,    73,    -1,    74,    -1,     5,    30,    70,    31,
      -1,    -1,    71,    -1,    70,    71,    -1,    24,    24,    34,
      -1,     6,    30,    31,    -1,     7,    30,    79,    31,    -1,
       7,    24,    30,    79,    31,    -1,     8,    30,    31,    -1,
      24,    24,    -1,    24,    -1,    76,    36,    24,    -1,    -1,
      78,    -1,    77,    36,    78,    -1,    24,    24,    -1,    24,
      24,    37,    94,    -1,    -1,    80,    -1,    79,    80,    -1,
      24,    34,    -1,    24,    83,    -1,    24,    32,    77,    33,
      34,    -1,    24,    32,    77,    33,    83,    -1,    76,    24,
      34,    -1,    76,    24,    83,    -1,    76,    24,    32,    77,
      33,    34,    -1,    76,    24,    32,    77,    33,    83,    -1,
      24,    44,    32,    78,    33,    34,    -1,    24,    44,    32,
      78,    33,    83,    -1,    24,    44,    34,    -1,    24,    44,
      83,    -1,    24,    44,    32,    33,    34,    -1,    24,    44,
      32,    33,    83,    -1,    82,    -1,    81,    -1,    24,   109,
      34,    -1,    24,   109,    83,    -1,    24,   109,    32,    33,
      34,    -1,    24,   109,    32,    33,    83,    -1,    24,   110,
      32,    78,    33,    34,    -1,    24,   110,    32,    78,    33,
      83,    -1,    30,    84,    31,    -1,    -1,    85,    -1,    84,
      85,    -1,    86,    34,    -1,    75,    34,    -1,    87,    34,
      -1,    88,    -1,    89,    -1,    90,    34,    -1,   101,    34,
      -1,    75,    37,    91,    -1,    92,    37,    91,    -1,    92,
      38,    91,    -1,    92,    39,    91,    -1,    92,    40,    91,
      -1,    92,    41,    91,    -1,    92,    42,    91,    -1,    14,
      32,    91,    33,    83,    -1,    88,    15,    32,    91,    33,
      83,    -1,    88,    16,    83,    -1,    17,    32,    91,    33,
      83,    -1,    13,   105,    -1,    92,    -1,    94,    -1,   100,
      -1,   101,    -1,   104,    -1,    12,    -1,    32,    91,    33,
      -1,    93,    -1,    24,    -1,    95,    -1,    96,    -1,    97,
      -1,    98,    -1,    99,    -1,    25,    -1,    26,    -1,    27,
      -1,    28,    -1,    29,    -1,    11,    24,    32,   105,    33,
      -1,    12,    35,    24,    32,   105,    33,    -1,    24,    32,
     105,    33,    -1,    91,    35,    24,    32,   105,    33,    -1,
      91,    18,    24,    -1,   102,    -1,   103,    -1,    44,    91,
      -1,    91,    48,    -1,    91,    49,    -1,    61,    91,    -1,
      58,    91,    -1,    91,    43,    91,    -1,    91,    44,    91,
      -1,    91,    45,    91,    -1,    91,    46,    91,    -1,    91,
      47,    91,    -1,    91,    50,    91,    -1,    91,    51,    91,
      -1,    91,    52,    91,    -1,    91,    53,    91,    -1,    91,
      54,    91,    -1,    91,    55,    91,    -1,    91,    56,    91,
      -1,    91,    57,    91,    -1,    91,    59,    91,    -1,    91,
      60,    91,    -1,    91,    62,    91,    -1,    91,    35,    24,
      -1,    -1,   106,    -1,   105,    36,   106,    -1,    91,    -1,
      44,    -1,    58,    -1,    61,    -1,    48,    -1,    49,    -1,
     107,    -1,   108,    -1,    43,    -1,    44,    -1,    45,    -1,
      46,    -1,    47,    -1,    50,    -1,    51,    -1,    52,    -1,
      53,    -1,    54,    -1,    55,    -1,    56,    -1,    57,    -1,
      59,    -1,    60,    -1,    62,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   192,   192,   193,   194,   197,   198,   201,   202,   203,
     206,   207,   208,   209,   212,   215,   216,   217,   220,   222,
     225,   226,   228,   231,   235,   236,   239,   240,   241,   244,
     245,   248,   249,   250,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     272,   273,   274,   275,   278,   279,   283,   286,   287,   288,
     291,   292,   293,   294,   295,   296,   297,   301,   304,   305,
     306,   307,   308,   309,   312,   313,   314,   317,   320,   323,
     324,   325,   326,   327,   328,   329,   332,   335,   338,   339,
     340,   341,   342,   345,   348,   351,   354,   357,   360,   363,
     364,   365,   366,   367,   368,   371,   373,   374,   376,   377,
     380,   381,   382,   383,   384,   386,   387,   388,   389,   390,
     391,   393,   394,   396,   397,   398,   402,   405,   406,   407,
     410,   413,   414,   415,   418,   419,   422,   423,   426,   427,
     428,   429,   430,   431,   432,   433,   434,   435,   436,   437,
     438,   439,   440,   441
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "KEYWORD_NATIVE", "KEYWORD_CLASS",
  "KEYWORD_DATA", "KEYWORD_EVENTS", "KEYWORD_INTERFACE",
  "KEYWORD_PROTECTED", "KEYWORD_GET", "KEYWORD_SET", "KEYWORD_NEW",
  "KEYWORD_THIS", "KEYWORD_RETURN", "KEYWORD_IF", "KEYWORD_ELSE_IF",
  "KEYWORD_ELSE", "KEYWORD_WHILE", "KEYWORD_AS", "KEYWORD_BOOLEAN",
  "KEYWORD_CHARACTER", "KEYWORD_INTEGER", "KEYWORD_FLOATING",
  "KEYWORD_STRING", "IDENTIFIER", "BOOLEAN_CONSTANT", "CHARACTER_CONSTANT",
  "INTEGER_CONSTANT", "FLOAT_CONSTANT", "STRING_CONSTANT",
  "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "LEFT_PARENTHESIS",
  "RIGHT_PARENTHESIS", "SEMICOLON", "DOT", "COMMA", "ASSIGN",
  "PLUS_ASSIGN", "MINUS_ASSIGN", "MULTIPLICATION_ASSIGN",
  "DIVISION_ASSIGN", "MODULO_ASSIGN", "PLUS", "MINUS", "MULTIPLICATION",
  "DIVISION", "MODULO", "INCREMENT", "DECREMENT", "EQUALS", "NOT_EQUALS",
  "GREATER", "GREATER_OR_EQUALS", "LOWER", "LOWER_OR_EQUALS",
  "LOGICAL_AND", "LOGICAL_OR", "LOGICAL_NEGATION", "BITWISE_AND",
  "BITWISE_OR", "BITWISE_NEGATION", "BITWISE_XOR", "UNARY_MINUS",
  "$accept", "classDefinitions", "classDefinition", "sections", "section",
  "dataSection", "memberDeclarations", "memberDeclaration",
  "eventsSection", "interfaceSection", "protectedSection",
  "variableDeclaration", "returnParameters", "methodParameters",
  "methodParameter", "methodDefinitions", "methodDefinition",
  "unaryOperatorDefinition", "binaryOperatorDefinition", "block",
  "statements", "statement", "variableDefinition", "assign", "if", "while",
  "return", "expression", "lValue", "identifier", "constant",
  "booleanConstant", "characterConstant", "integerConstant",
  "floatConstant", "stringConstant", "constructorCall", "methodCall",
  "unaryOperatorCall", "binaryOperatorCall", "propertyCall", "arguments",
  "argument", "prefixUnaryOperator", "postfixUnaryOperator",
  "unaryOperator", "binaryOperator", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    64,    65,    65,    65,    66,    66,    67,    67,    67,
      68,    68,    68,    68,    69,    70,    70,    70,    71,    72,
      73,    73,    74,    75,    76,    76,    77,    77,    77,    78,
      78,    79,    79,    79,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      81,    81,    81,    81,    82,    82,    83,    84,    84,    84,
      85,    85,    85,    85,    85,    85,    85,    86,    87,    87,
      87,    87,    87,    87,    88,    88,    88,    89,    90,    91,
      91,    91,    91,    91,    91,    91,    92,    93,    94,    94,
      94,    94,    94,    95,    96,    97,    98,    99,   100,   101,
     101,   101,   101,   101,   101,   102,   102,   102,   102,   102,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   104,   105,   105,   105,
     106,   107,   107,   107,   108,   108,   109,   109,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     2,     6,     5,     0,     1,     2,
       1,     1,     1,     1,     4,     0,     1,     2,     3,     3,
       4,     5,     3,     2,     1,     3,     0,     1,     3,     2,
       4,     0,     1,     2,     2,     2,     5,     5,     3,     3,
       6,     6,     6,     6,     3,     3,     5,     5,     1,     1,
       3,     3,     5,     5,     6,     6,     3,     0,     1,     2,
       2,     2,     2,     1,     1,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     5,     6,     3,     5,     2,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     5,     6,
       4,     6,     3,     1,     1,     2,     2,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     0,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     0,     3,     0,     0,     1,     4,     0,
       7,     7,     0,     0,     0,     0,     0,     8,    10,    11,
      12,    13,     0,    15,     0,     0,    31,     0,     6,     9,
       5,     0,     0,    16,    19,    31,    24,     0,     0,    32,
      49,    48,    22,     0,    14,    17,     0,    57,    26,    34,
     138,     0,   140,   141,   142,   134,   135,   143,   144,   145,
     146,   147,   148,   149,   150,   132,   151,   152,   133,   153,
      35,   136,   137,     0,     0,     0,     0,    20,    33,    18,
      21,     0,    84,   127,     0,     0,    87,    93,    94,    95,
      96,    97,     0,     0,     0,     0,     0,     0,    58,     0,
       0,    63,    64,     0,     0,    79,    86,    80,    88,    89,
      90,    91,    92,    81,    82,   103,   104,    83,     0,     0,
      27,     0,    44,    45,     0,    50,    51,     0,    26,    38,
      39,    25,     0,     0,    87,   130,    79,    82,    78,   128,
       0,     0,    23,   127,     0,   105,   109,   108,    61,     0,
      56,    59,    60,    62,     0,     0,    65,     0,     0,     0,
       0,     0,     0,     0,   106,   107,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    66,    29,     0,     0,     0,     0,     0,
       0,     0,   127,     0,     0,     0,     0,     0,    85,    67,
       0,    76,   102,   126,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
      68,    69,    70,    71,    72,    73,     0,    36,    37,    28,
      46,    47,     0,    52,    53,     0,     0,     0,   127,   129,
       0,     0,   100,     0,   127,    30,    42,    43,    54,    55,
      40,    41,    98,     0,    74,    77,     0,     0,    99,    75,
     101
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,    16,    17,    18,    32,    33,    19,    20,
      21,    96,    37,   119,   120,    38,    39,    40,    41,    70,
      97,    98,    99,   100,   101,   102,   103,   135,   136,   106,
     107,   108,   109,   110,   111,   112,   113,   137,   115,   116,
     117,   138,   139,    71,    72,    73,    74
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -143
static const yytype_int16 yypact[] =
{
       1,    21,    18,    97,  -143,    40,    36,  -143,  -143,    48,
     244,   244,    57,    73,   -21,    89,     6,  -143,  -143,  -143,
    -143,  -143,    74,   103,    58,   100,   108,   106,  -143,  -143,
    -143,   120,    39,  -143,  -143,   108,   360,    10,    59,  -143,
    -143,  -143,  -143,   111,  -143,  -143,    60,    27,   146,  -143,
    -143,    13,  -143,  -143,  -143,  -143,  -143,  -143,  -143,  -143,
    -143,  -143,  -143,  -143,  -143,  -143,  -143,  -143,  -143,  -143,
    -143,  -143,  -143,    94,   156,   101,   178,  -143,  -143,  -143,
    -143,   202,   177,   167,   215,   230,    37,  -143,  -143,  -143,
    -143,  -143,   167,   167,   167,   167,   -11,     4,  -143,   231,
     232,    78,  -143,   233,   329,   127,  -143,  -143,  -143,  -143,
    -143,  -143,  -143,  -143,   234,  -143,  -143,  -143,   240,   164,
    -143,    25,  -143,  -143,   254,  -143,  -143,   146,   146,  -143,
    -143,  -143,   257,   267,   258,   329,  -143,  -143,   256,  -143,
     167,   167,  -143,   167,   186,   259,   259,   259,  -143,   167,
    -143,  -143,  -143,  -143,   261,   265,  -143,   272,   273,   167,
     167,   167,   167,   167,  -143,  -143,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,  -143,   263,    65,   146,   152,   269,   153,
     270,   165,   167,   274,   167,   226,   266,   220,  -143,   329,
     167,  -143,  -143,   275,   168,   168,   -29,   -29,   -29,   409,
     409,   128,   128,   128,   128,    63,    63,   388,   388,   388,
     329,   329,   329,   329,   329,   329,   181,  -143,  -143,  -143,
    -143,  -143,   188,  -143,  -143,   190,   193,   221,   167,  -143,
     265,   265,  -143,   306,   167,  -143,  -143,  -143,  -143,  -143,
    -143,  -143,  -143,   222,  -143,  -143,   265,   227,  -143,  -143,
    -143
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -143,  -143,   295,   293,    70,  -143,  -143,   276,  -143,  -143,
    -143,  -143,  -143,   199,  -119,   294,    30,  -143,  -143,   -51,
    -143,   208,  -143,  -143,  -143,  -143,  -143,   -20,   -40,  -143,
     104,  -143,  -143,  -143,  -143,  -143,  -143,   -37,  -143,  -143,
    -143,  -142,   137,  -143,  -143,  -143,  -143
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
     123,   197,   188,    25,     1,     2,   158,   105,   190,    26,
     114,    12,    13,    14,    15,    81,    82,    83,    84,   164,
     165,    85,   126,   148,   130,     5,   149,   104,    86,    87,
      88,    89,    90,    91,    75,   150,    92,    28,    81,    82,
      83,    84,     6,    47,    85,   121,    76,   122,    93,   118,
     237,    86,    87,    88,    89,    90,    91,   105,   187,    92,
     114,   142,    94,    31,     9,    95,    10,   229,    78,   143,
      44,    93,   144,   145,   146,   147,    78,   104,    11,    12,
      13,    14,    15,    36,    36,    94,    29,    23,    95,    34,
      77,    80,    29,   154,   155,    47,   253,     7,   158,   227,
       1,     2,   257,    24,   201,    30,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,    27,
     195,   196,   174,   175,    47,   176,   124,    31,   125,   199,
      35,    47,    36,   128,   228,   129,   231,    42,   234,   204,
     205,   206,   207,   208,    43,    79,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   158,   177,   178,   179,   180,   181,   182,
     118,   159,   160,   161,   162,   163,   164,   165,    81,    82,
     243,   247,    47,    47,   249,   251,   230,   233,   127,   254,
     255,   134,    87,    88,    89,    90,    91,   185,   236,    92,
     186,   186,   131,   158,   157,   259,    87,    88,    89,    90,
      91,    93,   133,   161,   162,   163,   164,   165,    47,   198,
      47,   158,   246,    47,   248,    94,   132,   250,    95,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   157,   174,   175,   140,   176,    12,
      13,    14,    15,   242,   252,   258,   194,   194,   194,   240,
     260,   158,   141,   194,   184,   152,   153,   156,   183,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   157,   174,   175,   189,   176,   192,
     143,   193,   194,   200,   158,    47,   202,   203,     8,   241,
     226,   158,   232,   235,    22,   151,   238,   244,    45,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   157,   174,   175,   191,   176,    46,
     245,   239,     0,     0,     0,     0,     0,     0,     0,   256,
       0,   158,     0,     0,     0,     0,     0,   157,     0,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   158,   174,   175,     0,   176,     0,
       0,     0,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,     0,   174,   175,
      47,   176,    48,     0,    49,     0,     0,     0,     0,     0,
       0,     0,     0,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,   158,     0,     0,     0,     0,     0,     0,
       0,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   158,     0,     0,     0,     0,     0,
       0,     0,   159,   160,   161,   162,   163,   164,   165,     0,
       0,   168,   169,   170,   171
};

static const yytype_int16 yycheck[] =
{
      51,   143,   121,    24,     3,     4,    35,    47,   127,    30,
      47,     5,     6,     7,     8,    11,    12,    13,    14,    48,
      49,    17,    73,    34,    75,     4,    37,    47,    24,    25,
      26,    27,    28,    29,    24,    31,    32,    31,    11,    12,
      13,    14,    24,    30,    17,    32,    36,    34,    44,    24,
     192,    24,    25,    26,    27,    28,    29,    97,    33,    32,
      97,    24,    58,    24,    24,    61,    30,   186,    38,    32,
      31,    44,    92,    93,    94,    95,    46,    97,    30,     5,
       6,     7,     8,    24,    24,    58,    16,    30,    61,    31,
      31,    31,    22,    15,    16,    30,   238,     0,    35,    34,
       3,     4,   244,    30,   155,    31,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    30,
     140,   141,    59,    60,    30,    62,    32,    24,    34,   149,
      30,    30,    24,    32,   185,    34,   187,    31,   189,   159,
     160,   161,   162,   163,    24,    34,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,    35,    37,    38,    39,    40,    41,    42,
      24,    43,    44,    45,    46,    47,    48,    49,    11,    12,
     200,   232,    30,    30,   235,   236,    34,    34,    32,   240,
     241,    24,    25,    26,    27,    28,    29,    33,    33,    32,
      36,    36,    24,    35,    18,   256,    25,    26,    27,    28,
      29,    44,    35,    45,    46,    47,    48,    49,    30,    33,
      30,    35,    34,    30,    34,    58,    24,    34,    61,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    18,    59,    60,    32,    62,     5,
       6,     7,     8,    33,    33,    33,    36,    36,    36,    33,
      33,    35,    32,    36,    24,    34,    34,    34,    34,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    18,    59,    60,    33,    62,    32,
      32,    24,    36,    32,    35,    30,    24,    24,     3,    33,
      37,    35,    33,    33,    11,    97,    32,    32,    32,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    18,    59,    60,   128,    62,    35,
     226,   194,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    18,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    35,    59,    60,    -1,    62,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      30,    62,    32,    -1,    34,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    35,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    35,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    -1,
      -1,    52,    53,    54,    55
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    65,    66,     4,    24,     0,    66,    24,
      30,    30,     5,     6,     7,     8,    67,    68,    69,    72,
      73,    74,    67,    30,    30,    24,    30,    30,    31,    68,
      31,    24,    70,    71,    31,    30,    24,    76,    79,    80,
      81,    82,    31,    24,    31,    71,    79,    30,    32,    34,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      83,   107,   108,   109,   110,    24,    36,    31,    80,    34,
      31,    11,    12,    13,    14,    17,    24,    25,    26,    27,
      28,    29,    32,    44,    58,    61,    75,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,    24,    77,
      78,    32,    34,    83,    32,    34,    83,    32,    32,    34,
      83,    24,    24,    35,    24,    91,    92,   101,   105,   106,
      32,    32,    24,    32,    91,    91,    91,    91,    34,    37,
      31,    85,    34,    34,    15,    16,    34,    18,    35,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    59,    60,    62,    37,    38,    39,
      40,    41,    42,    34,    24,    33,    36,    33,    78,    33,
      78,    77,    32,    24,    36,    91,    91,   105,    33,    91,
      32,    83,    24,    24,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    91,    91,    91,    91,
      91,    91,    91,    91,    91,    91,    37,    34,    83,    78,
      34,    83,    33,    34,    83,    33,    33,   105,    32,   106,
      33,    33,    33,    91,    32,    94,    34,    83,    34,    83,
      34,    83,    33,   105,    83,    83,    33,   105,    33,    83,
      33
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 192 "beerlang.y"
    { bClasses = new ClassDefinitionList; ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 193 "beerlang.y"
    { bClasses = new ClassDefinitionList; bClasses->push_back((yyvsp[(1) - (1)].classDefinition)); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 194 "beerlang.y"
    { bClasses->push_back((yyvsp[(2) - (2)].classDefinition)); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 197 "beerlang.y"
    { (yyval.classDefinition) = new ClassDefinitionNode(true, *(yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].sections)); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 198 "beerlang.y"
    { (yyval.classDefinition) = new ClassDefinitionNode(*(yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].sections)); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 201 "beerlang.y"
    { (yyval.sections) = new Sections; ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 202 "beerlang.y"
    { (yyval.sections) = new Sections; (yyval.sections)->push_back((yyvsp[(1) - (1)].section)); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 203 "beerlang.y"
    { (yyval.sections) = (yyvsp[(1) - (2)].sections); (yyval.sections)->push_back((yyvsp[(2) - (2)].section)); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 206 "beerlang.y"
    { (yyval.section) = (yyvsp[(1) - (1)].section) ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 207 "beerlang.y"
    { (yyval.section) = (yyvsp[(1) - (1)].section) ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 208 "beerlang.y"
    { (yyval.section) = (yyvsp[(1) - (1)].section) ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 209 "beerlang.y"
    { (yyval.section) = (yyvsp[(1) - (1)].section) ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 212 "beerlang.y"
    { (yyval.section) = new DataSectionNode((yyvsp[(3) - (4)].memberDeclarations)); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 215 "beerlang.y"
    { (yyval.memberDeclarations) = new MemberDeclarationList; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 216 "beerlang.y"
    { (yyval.memberDeclarations) = new MemberDeclarationList; (yyval.memberDeclarations)->push_back((yyvsp[(1) - (1)].memberDeclaration)); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 217 "beerlang.y"
    { (yyval.memberDeclarations) = (yyvsp[(1) - (2)].memberDeclarations); (yyval.memberDeclarations)->push_back((yyvsp[(2) - (2)].memberDeclaration)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 220 "beerlang.y"
    { (yyval.memberDeclaration) = new MemberDeclarationNode(*(yyvsp[(1) - (3)].string), *(yyvsp[(2) - (3)].string)); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 222 "beerlang.y"
    { (yyval.section) = new EventsSectionNode; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 225 "beerlang.y"
    { (yyval.section) = new InterfaceSectionNode((yyvsp[(3) - (4)].methodDefinitions)); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 226 "beerlang.y"
    { (yyval.section) = new InterfaceSectionNode(*(yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].methodDefinitions)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 228 "beerlang.y"
    { (yyval.section) = new ProtectedSectionNode; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 231 "beerlang.y"
    { (yyval.variableDeclaration) = new VariableDeclarationNode(*(yyvsp[(1) - (2)].string), *(yyvsp[(2) - (2)].string)); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 235 "beerlang.y"
    { (yyval.returnParameters) = new StringList; (yyval.returnParameters)->push_back(*(yyvsp[(1) - (1)].string)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 236 "beerlang.y"
    { (yyval.returnParameters) = (yyvsp[(1) - (3)].returnParameters); (yyval.returnParameters)->push_back(*(yyvsp[(3) - (3)].string)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 239 "beerlang.y"
    { (yyval.methodParameters) = new MethodParameterList; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 240 "beerlang.y"
    { (yyval.methodParameters) = new MethodParameterList; (yyval.methodParameters)->push_back((yyvsp[(1) - (1)].methodParameter)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 241 "beerlang.y"
    { (yyval.methodParameters) = (yyvsp[(1) - (3)].methodParameters); (yyval.methodParameters)->push_back((yyvsp[(3) - (3)].methodParameter)); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 244 "beerlang.y"
    { (yyval.methodParameter) = new MethodParameterNode(*(yyvsp[(1) - (2)].string), *(yyvsp[(2) - (2)].string)); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 245 "beerlang.y"
    { (yyval.methodParameter) = new MethodParameterNode(*(yyvsp[(1) - (4)].string), *(yyvsp[(2) - (4)].string), (yyvsp[(4) - (4)].constant)); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 248 "beerlang.y"
    { (yyval.methodDefinitions) = new MethodDefinitionList; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 249 "beerlang.y"
    { (yyval.methodDefinitions) = new MethodDefinitionList; (yyval.methodDefinitions)->push_back((yyvsp[(1) - (1)].methodDefinition)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 250 "beerlang.y"
    { (yyval.methodDefinitions) = (yyvsp[(1) - (2)].methodDefinitions); (yyval.methodDefinitions)->push_back((yyvsp[(2) - (2)].methodDefinition)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 253 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (2)].string)); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 254 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (2)].string), (yyvsp[(2) - (2)].block)); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 255 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (5)].string), (yyvsp[(3) - (5)].methodParameters)); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 256 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (5)].string), (yyvsp[(3) - (5)].methodParameters), (yyvsp[(5) - (5)].block)); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 257 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode((yyvsp[(1) - (3)].returnParameters), *(yyvsp[(2) - (3)].string)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 258 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode((yyvsp[(1) - (3)].returnParameters), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].block)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 259 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode((yyvsp[(1) - (6)].returnParameters), *(yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].methodParameters)); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 260 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode((yyvsp[(1) - (6)].returnParameters), *(yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].methodParameters), (yyvsp[(6) - (6)].block)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 261 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (6)].string), *(yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].methodParameter), NULL); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 262 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (6)].string), *(yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].methodParameter), (yyvsp[(6) - (6)].block)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 263 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (3)].string), *(yyvsp[(2) - (3)].string)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 264 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (3)].string), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].block)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 265 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (5)].string), *(yyvsp[(2) - (5)].string)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 266 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (5)].string), *(yyvsp[(2) - (5)].string), (yyvsp[(5) - (5)].block)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 267 "beerlang.y"
    { (yyval.methodDefinition) = (yyvsp[(1) - (1)].methodDefinition); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 268 "beerlang.y"
    { (yyval.methodDefinition) = (yyvsp[(1) - (1)].methodDefinition); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 272 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (3)].string), *(yyvsp[(2) - (3)].string)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 273 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (3)].string), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].block)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 274 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (5)].string), *(yyvsp[(2) - (5)].string)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 275 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (5)].string), *(yyvsp[(2) - (5)].string), (yyvsp[(5) - (5)].block)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 278 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (6)].string), *(yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].methodParameter), NULL); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 279 "beerlang.y"
    { (yyval.methodDefinition) = new MethodDefinitionNode(*(yyvsp[(1) - (6)].string), *(yyvsp[(2) - (6)].string), (yyvsp[(4) - (6)].methodParameter), (yyvsp[(6) - (6)].block)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 283 "beerlang.y"
    {(yyval.block) = new BlockNode((yyvsp[(2) - (3)].statements)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 286 "beerlang.y"
    { (yyval.statements) = new Statements; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 287 "beerlang.y"
    { (yyval.statements) = new Statements; (yyval.statements)->push_back((yyvsp[(1) - (1)].statement)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 288 "beerlang.y"
    { (yyval.statements)->push_back((yyvsp[(2) - (2)].statement)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 291 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (2)].variableDefinition); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 292 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (2)].variableDeclaration); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 293 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (2)].statement); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 294 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (1)].ifStatement); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 295 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (1)].whileStatement); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 296 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (2)].statement); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 297 "beerlang.y"
    { (yyval.statement) = (yyvsp[(1) - (2)].methodCall); (yyvsp[(1) - (2)].methodCall)->setType(MethodCallNode::Statement); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 301 "beerlang.y"
    { (yyval.variableDefinition) = new VariableDefinitionNode((yyvsp[(1) - (3)].variableDeclaration), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 304 "beerlang.y"
    { (yyval.statement) = new AssignmentNode((yyvsp[(1) - (3)].lValue), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 305 "beerlang.y"
    { (yyval.statement) = new AssignmentNode((yyvsp[(1) - (3)].lValue), new MethodCallNode((yyvsp[(1) - (3)].lValue), "+", (yyvsp[(3) - (3)].expression))); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 306 "beerlang.y"
    { (yyval.statement) = new AssignmentNode((yyvsp[(1) - (3)].lValue), new MethodCallNode((yyvsp[(1) - (3)].lValue), "-", (yyvsp[(3) - (3)].expression))); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 307 "beerlang.y"
    { (yyval.statement) = new AssignmentNode((yyvsp[(1) - (3)].lValue), new MethodCallNode((yyvsp[(1) - (3)].lValue), "*", (yyvsp[(3) - (3)].expression))); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 308 "beerlang.y"
    { (yyval.statement) = new AssignmentNode((yyvsp[(1) - (3)].lValue), new MethodCallNode((yyvsp[(1) - (3)].lValue), "/", (yyvsp[(3) - (3)].expression))); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 309 "beerlang.y"
    { (yyval.statement) = new AssignmentNode((yyvsp[(1) - (3)].lValue), new MethodCallNode((yyvsp[(1) - (3)].lValue), "%", (yyvsp[(3) - (3)].expression))); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 312 "beerlang.y"
    { (yyval.ifStatement) = new IfNode((yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].block)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 313 "beerlang.y"
    { (yyval.ifStatement) = (yyvsp[(1) - (6)].ifStatement); (yyval.ifStatement)->addIfBlock((yyvsp[(4) - (6)].expression), (yyvsp[(6) - (6)].block)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 314 "beerlang.y"
    { (yyval.ifStatement) = (yyvsp[(1) - (3)].ifStatement); (yyval.ifStatement)->addElseBlock((yyvsp[(3) - (3)].block)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 317 "beerlang.y"
    { (yyval.whileStatement) = new WhileNode((yyvsp[(3) - (5)].expression), (yyvsp[(5) - (5)].block)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 320 "beerlang.y"
    { (yyval.statement) = new ReturnNode((yyvsp[(2) - (2)].arguments)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 323 "beerlang.y"
    { (yyval.expression) = (yyvsp[(1) - (1)].lValue); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 324 "beerlang.y"
    { (yyval.expression) = (yyvsp[(1) - (1)].constant); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 325 "beerlang.y"
    { (yyval.expression) = (yyvsp[(1) - (1)].constructorCall); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 326 "beerlang.y"
    { (yyval.expression) = (yyvsp[(1) - (1)].methodCall); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 327 "beerlang.y"
    { (yyval.expression) = (yyvsp[(1) - (1)].propertyCall); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 328 "beerlang.y"
    { (yyval.expression) = new ThisNode; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 329 "beerlang.y"
    { (yyval.expression) = (yyvsp[(2) - (3)].expression); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 332 "beerlang.y"
    { (yyval.lValue) = (yyvsp[(1) - (1)].identifier); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 335 "beerlang.y"
    { (yyval.identifier) = new IdentifierNode(*(yyvsp[(1) - (1)].string)); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 338 "beerlang.y"
    { (yyval.constant) = (yyvsp[(1) - (1)].constant); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 339 "beerlang.y"
    { (yyval.constant) = (yyvsp[(1) - (1)].constant); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 340 "beerlang.y"
    { (yyval.constant) = (yyvsp[(1) - (1)].constant); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 341 "beerlang.y"
    { (yyval.constant) = (yyvsp[(1) - (1)].constant); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 342 "beerlang.y"
    { (yyval.constant) = (yyvsp[(1) - (1)].constant); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 345 "beerlang.y"
    { (yyval.constant) = new BooleanConstantNode((yyvsp[(1) - (1)].boolean)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 348 "beerlang.y"
    { (yyval.constant) = new CharacterConstantNode((yyvsp[(1) - (1)].character)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 351 "beerlang.y"
    { (yyval.constant) = new IntegerConstantNode((yyvsp[(1) - (1)].integer)); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 354 "beerlang.y"
    { (yyval.constant) = new FloatConstantNode((yyvsp[(1) - (1)].floating)); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 357 "beerlang.y"
    { (yyval.constant) = new StringConstantNode(*(yyvsp[(1) - (1)].string)); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 360 "beerlang.y"
    { (yyval.constructorCall) = new ConstructorCallNode(*(yyvsp[(2) - (5)].string), (yyvsp[(4) - (5)].arguments)); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 363 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode(*(yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].arguments)); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 364 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode(*(yyvsp[(1) - (4)].string), (yyvsp[(3) - (4)].arguments)); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 365 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (6)].expression), *(yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].arguments)); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 366 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(3) - (3)].string)); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 367 "beerlang.y"
    { (yyval.methodCall) = (yyvsp[(1) - (1)].methodCall) ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 368 "beerlang.y"
    { (yyval.methodCall) = (yyvsp[(1) - (1)].methodCall) ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 371 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(2) - (2)].expression), *(yyvsp[(1) - (2)].string)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 373 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].string)); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 374 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (2)].expression), *(yyvsp[(2) - (2)].string)); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 376 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(2) - (2)].expression), *(yyvsp[(1) - (2)].string)); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 377 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(2) - (2)].expression), *(yyvsp[(1) - (2)].string)); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 380 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 381 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 382 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 383 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 384 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 386 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 387 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 388 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 389 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 390 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 391 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 393 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 394 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 396 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 397 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 398 "beerlang.y"
    { (yyval.methodCall) = new MethodCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(2) - (3)].string), (yyvsp[(3) - (3)].expression)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 402 "beerlang.y"
    { (yyval.propertyCall) = new PropertyCallNode((yyvsp[(1) - (3)].expression), *(yyvsp[(3) - (3)].string)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 405 "beerlang.y"
    { (yyval.arguments) = new ExpressionList; ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 406 "beerlang.y"
    { (yyval.arguments) = new ExpressionList; (yyval.arguments)->push_back((yyvsp[(1) - (1)].expression)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 407 "beerlang.y"
    { (yyval.arguments) = (yyvsp[(1) - (3)].arguments); (yyval.arguments)->push_back((yyvsp[(3) - (3)].expression)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 410 "beerlang.y"
    { (yyval.expression) = (yyvsp[(1) - (1)].expression); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 413 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 414 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 415 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 418 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 419 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 422 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 423 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 426 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 427 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 428 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 429 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 430 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 431 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 432 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 433 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 434 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 435 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 436 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 437 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 438 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 439 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 440 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 441 "beerlang.y"
    { (yyval.string) = (yyvsp[(1) - (1)].string); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2842 "beerlang.tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 443 "beerlang.y"

