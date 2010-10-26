#include "radio/radio.h"

void init_payload();
void ping_init(protocol_t protocol, uint8_t channr, radio_address_t addr);
void ping(radio_address_t addr, uint8_t channr);
void calc_rtt();
void print_success();
void print_failed();
void gpio_n_timer_init();
void adjust_timer();
static void ping_handler(void *payload, int payload_size, 
					packet_info_t *packet_info);
static void pong_handler(void *payload, int payload_size, 
                                        packet_info_t *packet_info);
void pong(uint16_t src);

typedef struct pong{
	int hopcount;
	int ttl;
	radio_address_t radio_address;
} ping_r;

typedef struct ping_payload{
	char* payload;
} ping_payload;