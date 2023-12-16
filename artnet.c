#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "common.h"
#include "config.h"
#include "artnet.h"
#include "ws2812.h"

// local prototypes
void artnet_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

// UDP holder
struct udp_pcb *pcb;

// Up to ARTNET_MAX_UNIVERSES * 512 bytes hold a single channel
struct ArtDmx artdmx[ARTNET_MAX_UNIVERSES];
uint8_t artdmx_index = 0;
uint8_t p_sequence = 0;

// Holder for callback function
static void(*onDataAvailable)(void *) = NULL;
static uint8_t nrArtPollResponses = 0;

//
struct ArtNetStatus art_status = {false, 0};
struct ArtNetStatus artnet_status(void){
	return art_status;
}

/*
 * ArtPoll received, reply with ArtPollReply
 */
void handleArtDiscovery(uint8_t *data, struct ArtPollReply *reply) {
	
	// Get the Artpoll ...
	struct ArtPoll artpoll = {};
	strncpy(artpoll.ID, (char*)data, 8);
	artpoll.OpCode = (data[8] + (data[9]<<8));	
	artpoll.ProtVerHi = data[10];	
	artpoll.ProtVerLo = data[11];	
	artpoll.Flags = data[12];
	
	// ... wait random 100ms < t < 500ms according protocol
	sleep_ms( (rand()%(500-100+1))+100);

	// Construct ArtPollReply
	strcpy(reply->ID,"Art-Net");
	reply->OpCode = 0x2100; // 0x2100, LSB first
	uint32_t ip = ip4_addr_get_u32(netif_ip4_addr(netif_list));
	reply->IPAddr[3] = (ip >> 24) & 0xFF;
	reply->IPAddr[2] = (ip >> 16) & 0xFF;
	reply->IPAddr[1] = (ip >>  8) & 0xFF;
	reply->IPAddr[0] = (ip >>  0) & 0xFF;
	reply->Port = 0x1936; 	// 0x1936, LSB first
	reply->VersInfoH = 0x01;
	reply->VersInfoL = 0x00;
	strcpy(reply->ShortName, "PicoPixelz");
	strcpy(reply->LongName, "PicoPixelz by dkroeske@gmail.com");
	sprintf(reply->NodeReport, "#0001 %.4d 0001 by dkroeske@gmail.com", nrArtPollResponses++);
	reply->NumPortsHi = 0x00; // Always zero
	reply->NumPortsLo = 0x01; // Always zero
	reply->PortTypes[0] = 0x80; // 1 input, DMX512
	
	reply->SwOut[0] = config_get_universe(); // Xlights presents universe 
}

/*
 * Callback incomming UDP datagrams
 */
void artnet_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {

	uint8_t *data = (unsigned char*)p->payload;
	
	// 
	uint16_t OpCode = ((data[9]<<8)+data[8]);
	switch( OpCode ) {
		// ArtPoll
		case 0x2000: {
			// Construct ArtNet reply
			struct ArtPollReply reply = {};
			handleArtDiscovery(data, &reply);
			
			// UDP transmit to caller of ArtPoll
			cyw43_arch_lwip_begin();
			struct pbuf *tx_buff = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct ArtPollReply), PBUF_RAM);
			memcpy((uint8_t *)tx_buff->payload, (uint8_t *)&reply, sizeof(struct ArtPollReply));
			udp_sendto(pcb, tx_buff, addr, ARTNET_DISCOVERY_PORT);
			pbuf_free(tx_buff);
			cyw43_arch_lwip_end();
		}	
		break;

		// ArtDmx
		case 0x5000: {
			// Get and covert rx data to ArtDmx			
			struct ArtDmx dmx = {};
			memcpy( (uint8_t *)&dmx, data, sizeof(struct ArtDmx) );

			// Get and check if ArtDmx in within universe
			int universe = config_get_universe();
			if( (dmx.SubUni >= universe) && (dmx.SubUni < (universe+ARTNET_MAX_UNIVERSES)) ) {
			
				// If so play leds every time sequence changes. Save multiple 512 DMX-
				// frames
				if( p_sequence != dmx.Sequence ) {
					// For all leds map led-number to 512 dmx buffers
					unsigned int nr_leds = config_get_nr_leds();
					for(uint16_t idx = 0; idx < nr_leds*3; idx+=3) {
						uint8_t r,g,b;
						r = artdmx[idx/512].Data[idx%512];
						g = artdmx[(idx+1)/512].Data[(idx+1)%512];
						b = artdmx[(idx+2)/512].Data[(idx+2)%512];
						pixel(r,g,b);
					}

//					for(uint8_t idx = 0; idx < artdmx_index+1; idx++) {
//						printf("SubUni %.2d: Sequence: %.2d\n", artdmx[idx].SubUni, artdmx[idx].Sequence);
//					}
					
					artdmx_index = 0;
					artdmx[artdmx_index] = dmx;
				} else {
					if(artdmx_index < ARTNET_MAX_UNIVERSES) {
						artdmx_index++ ;
						artdmx[artdmx_index] = dmx;
					}
				}	
				p_sequence = dmx.Sequence;
			}

		}
		break;	
	}

	// Mandatory
	pbuf_free(p);


	// Update time
	art_status.dirty = true;
	art_status.last_update = time_us_64();
}

/*
 * Init UDP interface
 */
int initArtNet( void(*cb)(void *) )
{
	// Save callback 
	onDataAvailable = cb;

	// Setup new UDP connection
	pcb = udp_new();

	// Bind to port Artnet discovery port
  	err_t err = udp_bind(pcb, IP_ADDR_ANY, ARTNET_DISCOVERY_PORT);
	
	// Start receiver and set callback if all is ok
	if(ERR_OK == err) {
		udp_recv(pcb, artnet_recv, NULL);
	}
	
	if(ERR_OK == err) {
		return OK;
	} else {
		return NOK;
	}
}
