%{

#include <stdio.h>
#include <string.h>
#include "config_parser.h"

%}

%option 8bit
%option bison-bridge
%option case-insensitive
%option ecs
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
%option yylineno

DIGIT		[0-9]		
ID		\"[^"\n]*["\n]

%%

\[wifi\]	return WIFI_TOK;
\[dmx\]		return DMX_TOK;
\[idle\]	return IDLE_TOK;
ssid		return WIFI_SSID;
password	return WIFI_PASSWORD;
universe	return DMX_UNIVERSE;
nrleds		return DMX_NRLEDS;
enabled		return IDLE_ENABLED;
filename	return IDLE_FILENAME;
framerate	return IDLE_FRAMERATE;

\=	        return EQUAL;
{DIGIT}+	yylval->number = atoi(yytext); return NUMBER;
{ID}		{
		    yylval->string = strdup(yytext+1); 
		    yylval->string[yyleng-2] = '\0';
		    return ID;
		}
[#;].*\n	; /* Eat up all single line comments starting with ; of # */
[ \t]+		; /* Eat up all whitespace */
[\n]		; /* ignore eols */
.		; 

%%

