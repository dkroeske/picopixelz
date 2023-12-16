%code {

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_lexer.h"

int yyerror(CONFIG_STRUCT *config, char *msg);

}

%code requires {
	typedef struct {
		// [wifi]
		char wifi_ssid[50];
		char wifi_password[50];
		// [dmx]
		unsigned char dmx_universe;
		unsigned int  dmx_nrleds;
		// [idle]
		unsigned char idle_enabled;
		char idle_filename[20];
		unsigned char idle_framerate;
		
	} CONFIG_STRUCT;
}

%define api.pure true

%parse-param {CONFIG_STRUCT *config}

%union {
	int number;
	char *string;
}

%token<number> NUMBER 
%token<string> ID 
%token WIFI_TOK WIFI_SSID WIFI_PASSWORD 
%token DMX_TOK DMX_UNIVERSE DMX_NRLEDS
%token IDLE_TOK IDLE_ENABLED IDLE_FILENAME IDLE_FRAMERATE
%token QUOTE OBRAC EBRAC EQUAL

%start config

%%

config:		%empty
		|
		config WIFI_TOK wifi_content
		|
		config DMX_TOK dmx_content
		|
		config IDLE_TOK idle_content
;

wifi_content:	%empty
		|
		wifi_content WIFI_SSID EQUAL ID 
		{ 
			strcpy(config->wifi_ssid, yylval.string);
		}
		|
		wifi_content WIFI_PASSWORD EQUAL ID 
		{ 
			strcpy(config->wifi_password, yylval.string);
		}
;

dmx_content:	%empty
		|
		dmx_content DMX_UNIVERSE EQUAL NUMBER 
		{ 
			config->dmx_universe = yylval.number;
		}
		|
		dmx_content DMX_NRLEDS EQUAL NUMBER 
		{ 
			config->dmx_nrleds = yylval.number;
		}
;

idle_content:	%empty
		|
		idle_content IDLE_ENABLED EQUAL NUMBER 
		{ 
			config->idle_enabled = yylval.number;
		}
		|
		idle_content IDLE_FILENAME EQUAL ID 
		{ 
			strcpy(config->idle_filename, yylval.string);
		}
		|
		idle_content IDLE_FRAMERATE EQUAL NUMBER 
		{ 
			config->idle_framerate = yylval.number;
		}
;


%%

int yyerror(CONFIG_STRUCT *config, char *msg){
	fprintf(stderr, "error: %s\n", msg);
	return 0;
}

