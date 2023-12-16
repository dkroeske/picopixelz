#ifndef CONFIG_INCLUDE
#define CONFIG_INCLUDE

int config_init(const char *file_name);
char* config_get_ssid(void);
char* config_get_password(void);
int config_get_universe(void);
int config_get_nr_leds(void); 

void config_show(void);
#endif
