#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include "common.h"
#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "config.h"
#include "config_parser.h"
#include "config_lexer.h"

CONFIG_STRUCT config;

char* config_get_ssid(void){
	return config.wifi_ssid;
}

char* config_get_password(void){
	return config.wifi_password;
}

int config_get_universe(void){
	return config.dmx_universe;
}

int config_get_nr_leds(void) {
	return config.dmx_nrleds;
}

int config_get_idle_enable(void) {
	return config.dmx_nrleds;
}

CONFIG_STRUCT get_config() {
	return config;
}


//
// Read config from sd-card
//
int config_init(const char *file_name) {

	int retval = OK;

	// Open config file and parse config
	if( OK == retval ) {

		FIL fp;
		FRESULT fr = f_open(&fp, file_name, FA_READ);
		if( FR_OK != fr ) {
			retval = NOK;
		} else {
		
			char buf[1024] = "";
			char line_buf[80];
			
			while( NULL != f_gets(line_buf, 80, &fp) ) {
				strcat(buf, line_buf);
			}
			
			YY_BUFFER_STATE bs = yy_scan_buffer(buf, 1024);
			yy_switch_to_buffer(bs);
			yyparse(&config);
			
		} 
		f_close(&fp);
	} 
	
	return retval;
}

//
// Show config
//
void config_show(void) {

	/* Print config */
	printf("---- CONFIG ----\n");
	printf("wifi ssid:%s\n", config.wifi_ssid);
	printf("wifi password:%s\n", config.wifi_password);
	printf("dmx universe:%d\n", config.dmx_universe);
	printf("dmx nrleds:%d\n", config.dmx_nrleds);
	printf("idle filename:%s\n", config.idle_filename);
	printf("idle framerate:%d\n", config.idle_framerate);
	printf("idle enabled:%d\n", config.idle_enabled);
	printf("\n");
}
