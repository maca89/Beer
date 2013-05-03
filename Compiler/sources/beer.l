%{
	#pragma once

	#include "stdafx.h"

	#include "Beer.tab.hpp"
	#include "Output.h"

	std::stringstream ss;

	size_t lineNumber = 1;
	std::string filename;
	Beer::Output * out;

	#define parse(var) ss << yytext; ss >> var; ss.clear();
%}

%option noyywrap


NUM				[0-9]
ALPHA			[a-zA-Z]
ALPHANUM		[0-9a-zA-Z]

%%

native							{ return KEYWORD_NATIVE; }
class							{ return KEYWORD_CLASS; }

data							{ return KEYWORD_DATA; }
events							{ return KEYWORD_EVENTS; }
interface						{ return KEYWORD_INTERFACE; }
protected						{ return KEYWORD_PROTECTED; }

new								{ return KEYWORD_NEW; }
this							{ return KEYWORD_THIS; }
return							{ return KEYWORD_RETURN; }
if								{ return KEYWORD_IF; }
elseif							{ return KEYWORD_ELSE_IF; }
else							{ return KEYWORD_ELSE; }
while							{ return KEYWORD_WHILE; }
as								{ return KEYWORD_AS; }

bool							{ yylval.string = new std::string("bool"); return KEYWORD_BOOLEAN; }
char							{ yylval.string = new std::string("char"); return KEYWORD_CHARACTER; }
int								{ yylval.string = new std::string("int"); return KEYWORD_INTEGER; }
float							{ yylval.string = new std::string("float"); return KEYWORD_FLOATING; }
string							{ yylval.string = new std::string("string"); return KEYWORD_STRING; }



true							{ yylval.boolean = true; return BOOLEAN_CONSTANT; }
false							{ yylval.boolean = false; return BOOLEAN_CONSTANT; }
'.'								{ yylval.character = yytext[1]; return CHARACTER_CONSTANT; }
-?{NUM}+						{ parse(yylval.integer); return INTEGER_CONSTANT; }
-?{NUM}+\.{NUM}+				{ parse(yylval.floating); return FLOAT_CONSTANT; }
\"[^\"]*\"						{ yylval.string = new std::string(yytext + 1, strlen(yytext) - 2); return STRING_CONSTANT; }

{ALPHA}{ALPHANUM}*				{ yylval.string = new std::string(yytext); return IDENTIFIER; }

\{								{ return LEFT_CURLY_BRACKET; }
\}								{ return RIGHT_CURLY_BRACKET; }
\(								{ return LEFT_PARENTHESIS; }
\)								{ return RIGHT_PARENTHESIS; }

;								{ return SEMICOLON; }
\.								{ return DOT; }
,								{ return COMMA; }

=								{ return ASSIGN; }
\+=								{ return PLUS_ASSIGN; }
-=								{ return MINUS_ASSIGN; }
\*=								{ return MULTIPLICATION_ASSIGN; }
\/=								{ return DIVISION_ASSIGN; }
%=								{ return MODULO_ASSIGN; }

\+								{ yylval.string = new std::string("+"); return PLUS; }
-								{ yylval.string = new std::string("-"); return MINUS; }
\*								{ yylval.string = new std::string("*"); return MULTIPLICATION; }
\/								{ yylval.string = new std::string("/"); return DIVISION; }
%								{ yylval.string = new std::string("%"); return MODULO; }

\+\+							{ yylval.string = new std::string("++"); return INCREMENT; }
\-\-							{ yylval.string = new std::string("--"); return DECREMENT; }

==								{ yylval.string = new std::string("=="); return EQUALS; }
\!=								{ yylval.string = new std::string("!="); return NOT_EQUALS; }
\>								{ yylval.string = new std::string(">"); return GREATER; }
\>=								{ yylval.string = new std::string(">="); return GREATER_OR_EQUALS; }
\<								{ yylval.string = new std::string("<"); return LOWER; }
\<=								{ yylval.string = new std::string("<="); return LOWER_OR_EQUALS; }

&&								{ yylval.string = new std::string("&&"); return LOGICAL_AND; }
\|\|							{ yylval.string = new std::string("||"); return LOGICAL_OR; }
!								{ yylval.string = new std::string("!"); return LOGICAL_NEGATION; }

&								{ yylval.string = new std::string("&"); return BITWISE_AND; }
\|								{ yylval.string = new std::string("|"); return BITWISE_OR; }
~								{ yylval.string = new std::string("~"); return BITWISE_NEGATION; }
\^								{ yylval.string = new std::string("^"); return BITWISE_XOR; }

\/\/[^\n]*						;
[ \t\r]*						;
[ \n]							{ lineNumber++; }

.								{
									ss << lineNumber;
									std::string ln;
									ss >> ln;
									ss.clear();
									out->addError("File " + filename + " could not be parsed - unknown character: " + yytext + "  on line number " + ln);
								}
%%