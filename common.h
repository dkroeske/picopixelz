#ifndef COMMON_INC
#define COMMON_INC

#define NOK 0 
#define OK  1

// DMX_datapacket
typedef struct {
	uint8_t size;
	uint8_t *datap;
} DMX_DATAPACKET_STRUCT;

#endif
