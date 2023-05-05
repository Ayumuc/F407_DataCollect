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
#include <time.h>
#include <sys/time.h>
#include <list>
#include <mutex>
#include <algorithm>
#include <map>
#include <fstream>
#include <cassert>
#include "../inc/json/json.h"
#include "../inc/json/json-forwards.h"
#include <cstdlib>
#include <thread>
#include <list>
#include <vector>
#include "../inc/SafeQueue.h"
using namespace std;
#define PRINT 0
void Tcp_client_init(string Client_ip);
void* Usart_init (void*arg);
void Tcpsever_init();
void* Tcp_client2_init(void* arg);
uint8_t* parseModbusCommand(uint8_t *modbusCmd, int32_t comLength, uint8_t* size);

float callback_decode_Ax(uint8_t* buf);
float callback_decode_Ay(uint8_t* buf);
float callback_decode_Az(uint8_t* buf);
float callback_decode_Ax2(uint8_t* buf);
float callback_decode_Ay2(uint8_t* buf);
float callback_decode_Az2(uint8_t* buf);
void Accleration_decode(uint8_t* buf);
void Accleration_decode2(uint8_t* buf);
void Client_ip_init();
void Common_data_set();
string string_to_hex(const string& str);
void Tcp_sever_init(string Sever_Port);
void config_read(void);
void data_insert(int i , string Client_ip);
static unsigned short int getModbusCRC16(unsigned char *_pBuf, unsigned short int _usLen);
typedef struct usart_data{
    uint8_t data_head;
    uint8_t data_key;
    uint8_t data_value[4];
    uint8_t data_crc8;
    uint8_t data_end;
}DATA;
extern char speed_value[4];
extern char Ax_Handle_buffer[50];
extern char Ay_Handle_buffer[50];
extern char Az_Handle_buffer[50];
extern int data_pos;



class Common_Data {
    public:
            string Factory_type;
            string Factroy_IP;
};




class My_Factory{
    public:
        std::string Factory_Type;
        std::string Factroy_Num;
};

extern Common_Data common_data;
extern map<string,string> Factory_map;
extern map<string,double> Factory_data_map;
extern My_Factory my_Factory;





#endif /* __MAIN_H */