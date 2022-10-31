#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <assert.h>
#include <string>
#define PRINT 0
void* Tcp_client_init(void* arg);
void* Usart_init (void*arg);
void* Tcpsever_init(void*arg);
uint8_t* parseModbusCommand(uint8_t *modbusCmd, int32_t comLength, uint8_t* size);
extern uint8_t Tcp_client_callbackbuf[128];
void Accleration_decode(uint8_t* buf);
typedef struct usart_data{
    uint8_t data_head;
    uint8_t data_key;
    uint8_t data_value[4];
    uint8_t data_crc8;
    uint8_t data_end;
}DATA;
extern char speed_value[4];
extern int data_pos;
#endif /* __MAIN_H */