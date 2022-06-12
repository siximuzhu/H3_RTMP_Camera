#ifndef __HEADER_H
#define __HEADER_H

#include <stdint.h>

int asd1013_init(void);
int get_power(int fd);

int ir_init();
void ir_send(int fd,unsigned char *data,unsigned int len);
int ir_recv(int fd,unsigned char *data,unsigned int len);

void nrf24l01_init();
int NRF24L01_RxPacket(uint8_t *rxbuf);

int http_post_data(int counter,int power);
#endif
