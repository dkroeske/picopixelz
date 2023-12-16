#include <stdlib.h>

#ifndef ARTNET_INC
#define ARTNET_INC

#define ARTNET_DISCOVERY_PORT 6454
#define ARTNET_MAX_UNIVERSES	8

int initArtNet( void(*cb)(void *) );

struct ArtNetStatus {
	bool dirty;
	uint64_t	last_update;
};

struct ArtNetStatus artnet_status(void);


struct ArtPoll {
	char ID[8];
	uint16_t OpCode;
	uint8_t ProtVerHi;
	uint8_t ProtVerLo;
	uint8_t Flags;
};

struct ArtPollReply {
	char ID[8]; 
	uint16_t OpCode;
	uint8_t IPAddr[4];
	uint16_t Port;
	uint8_t VersInfoH;
	uint8_t VersInfoL;
	uint8_t NetSwitch;
	uint8_t SubSwitch; 
	uint8_t OemHi;
	uint8_t OemLo;
	uint8_t UbeaVersion;
	uint8_t Status1;
	uint8_t EstaManLo;
	uint8_t EstaManHi;
	char ShortName[18];
	char LongName[64];
	char NodeReport[64];
	uint8_t NumPortsHi;
	uint8_t NumPortsLo;
	uint8_t PortTypes[4];
	uint8_t GoodInput[4];
	uint8_t GoodInputA[4];
	uint8_t SwIn[4];
	uint8_t SwOut[4];
	uint8_t AcnPriority;
	uint8_t SwMacro;
	uint8_t SwRemote;
	uint8_t Spare[3];
	uint8_t Style;
	uint8_t MAC[6];
	uint8_t BindIp[4];
	uint8_t BindIndex;
	uint8_t Status2;
	uint8_t GoodOutputB[4];
	uint8_t Status3;
	uint8_t Filler[15];
};

struct ArtDmx {
	char ID[8];
	uint16_t OpCode;
	uint8_t ProtVerHi;
	uint8_t ProtVerLo;
	uint8_t Sequence;
	uint8_t Physical;
	uint8_t SubUni;
	uint8_t Net;
	uint8_t LengthHi;
	uint8_t Length;
	uint8_t Data[512];
};

#endif
