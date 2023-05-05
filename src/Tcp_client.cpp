/*
Author:LZH
TARGET:TCP客户端获取RS232/485信息然后将获取的信息保存起来，让TCP服务端调用
TIME:2022.10.14
*/
#include "../inc/main.h"
#include "../inc/Acceleration.h"
#include<stdio.h>  
#include<stdlib.h>  
#include<unistd.h>  
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>  
#include<termios.h>  
#include<errno.h>  
#include <time.h>
#include "../inc/data_collect.h"


uint8_t Tcp_client_before_buffer[11];
uint8_t Tcp_client_callbackbuf[128];
#define SCALE (50.0/16)
#define I_min (4)
#define PRINTF 1
uint8_t Tcp_client_crc[19] = {0};
uint16_t CRC_S = 0;
void Accleration_decode(uint8_t* buf);
void Tcp_client_init(string Client_ip){

    //打印数据确认IP地址正确
    //std::cout<< Client_ip << std::endl;
    //printf("ip:%s\n",Client_ip.c_str());
    int ret = 0;
    int Sum = 0;
    uint16_t origin_data = 0;
    uint8_t Tcp_client_buffer[21] = {0};
    uint8_t Tcp_client_error_buffer[21] = {0};
    uint8_t crc_sum = 0;
    string IP_N = "IP0";
    string channel = "Channel0";
    double My_common_data = 0;
    int My_bit = 3;
    int count = 0 ;
    double out = 0 ;
    int flags = 0;
    int res;
    char write_buffer[128] = {0};
    int fd = 0;
    string db_txt = ("./db/" + Client_ip + ".txt");
    cout << "txt : " << db_txt << endl;
    fd = open(db_txt.c_str(),O_RDWR|O_CREAT);
    if(fd < 0){
        cout << "open " << db_txt <<" error" << endl;
    }
    for(int i = 0 ; i < sizeof(Tcp_client_error_buffer) ; i++){
        Tcp_client_error_buffer[i] = 0x00;
    }
//  创建一个通信的套接字，需要指定服务器的IP和端口号
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd != -1);

    struct sockaddr_in ser_addr;
    


//连接服务器，需要知道服务器绑定的IP和端口
    while(1){  

        disconnect:
        sockfd = socket(AF_INET,SOCK_STREAM,0);
        memset(&ser_addr,0,sizeof(ser_addr));
        struct timeval timeout = {10,0};
        setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));//设置接收超时
        ser_addr.sin_family = AF_INET;
        ser_addr.sin_port =htons(4110);//转化端口号
        ser_addr.sin_addr.s_addr = inet_addr(Client_ip.c_str());//回环地址

        res = connect(sockfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
        if((res < 0)){
            printf("connect %s error!\n",Client_ip.c_str());
            goto disconnect;
        }
        else{
            printf("connect %s success!\n",Client_ip.c_str());
        }
        while(1){
            memset(Tcp_client_callbackbuf,0,sizeof(Tcp_client_callbackbuf));
            int len = recv(sockfd,Tcp_client_callbackbuf,sizeof(Tcp_client_callbackbuf),MSG_WAITALL);
            if(len > 0 && len < 14){printf("len = %d\n",len);continue;}
            else if(len <= 0){
                printf("recv error!\n");
                goto disconnect;
            }
            
            if(Tcp_client_callbackbuf[0] == 0){continue;}

            for(int i = 0 ; i < (sizeof(Tcp_client_callbackbuf) - 21) ; i++){
                if(Tcp_client_callbackbuf[i] == 0x01 && Tcp_client_callbackbuf[i + 1] == 0x04 && Tcp_client_callbackbuf[i + 2] == 0x10 ){
                    for(int j = 0 ; j < 21 ; j++){
                        Tcp_client_buffer[j] = Tcp_client_callbackbuf[i+j];
                    }
                    for(int k = 0 ; k < 19 ; k++){
                        Tcp_client_crc[k] = Tcp_client_buffer[k];
                    } 
                    //校验
                    CRC_S = getModbusCRC16(Tcp_client_crc,sizeof(Tcp_client_crc));
                    if(CRC_S == (Tcp_client_buffer[20] << 8 | Tcp_client_buffer[19])){
                      //校验成功
                      for(int n = 0 ; n < atoi(Factory_map["IP_NUM"].c_str()) ; n++){
                        string::reverse_iterator it = IP_N.rbegin();
                        *it = (( n + 1 ) + '0');
                        if(Factory_map[IP_N].compare(Client_ip) == 0){
                            switch(flags){
                                case 0:
                                for(int h = 0 ; h < atoi(Factory_map[("Channel"+to_string(n+1))].c_str()) ; h++ ){
                                    origin_data = ((Tcp_client_buffer[(2*h)+3] << 8) | Tcp_client_buffer[(2*h)+4]);
                                    My_common_data = origin_data;
                                    out = ((My_common_data/1024*5/200*1000) - I_min)*SCALE;
                                    Factory_data_map.insert(pair<string,double>((("IP"+to_string(n+1))+("Channel"+to_string(h+1))),out));
                                }
                                flags = 1;
                                break;
                                case 1:
                                for(int h = 0 ; h < atoi(Factory_map[("Channel"+to_string(n+1))].c_str()) ; h++ ){
                                    origin_data = ((Tcp_client_buffer[(2*h)+3] << 8) | Tcp_client_buffer[(2*h)+4]);
                                    My_common_data = origin_data;
                                    out = ((My_common_data/1024*5/200*1000) - I_min)*SCALE;
                                    Factory_data_map[(("IP"+to_string(n+1))+("Channel"+to_string(h+1)))] = out;
                                }
                                break;
                                default:break;
                            }
                            for(auto iy = Factory_data_map.begin(); iy != Factory_data_map.end() ; iy++){
                                cout << "key:" << iy->first <<"   value:" << iy->second << endl;
                            }
                        }
                      }
                    }
                    break;
                }
            }

            /*获取当前时间并写入日志*/
            struct tm *ptm = NULL;
            struct timeval tv;
            struct timezone tz;
            short nMilSec = 0;
            gettimeofday(&tv, &tz); 
            nMilSec = (long long)tv.tv_usec/1000;
            ptm = localtime(&tv.tv_sec); 
            sprintf(write_buffer,"%04d-%02d-%02d  %02d:%02d:%02d:%03d : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",ptm->tm_year + 1900, ptm->tm_mon + 1 , ptm->tm_mday , ptm->tm_hour , ptm->tm_min , ptm->tm_sec , nMilSec , Tcp_client_buffer[0],Tcp_client_buffer[1],Tcp_client_buffer[2],Tcp_client_buffer[3],Tcp_client_buffer[4],Tcp_client_buffer[5],Tcp_client_buffer[6],Tcp_client_buffer[7],Tcp_client_buffer[8],Tcp_client_buffer[9],Tcp_client_buffer[10],Tcp_client_buffer[11],Tcp_client_buffer[12],Tcp_client_buffer[13],Tcp_client_buffer[14],Tcp_client_buffer[15],Tcp_client_buffer[16],Tcp_client_buffer[17],Tcp_client_buffer[18],Tcp_client_buffer[19],Tcp_client_buffer[20]);
            //printf("%s\n",write_buffer);
            write(fd,write_buffer,sizeof(write_buffer));
            memset(Tcp_client_buffer,0,sizeof(Tcp_client_buffer));
            memset(Tcp_client_callbackbuf,0,sizeof(Tcp_client_callbackbuf)); 
        }
        sleep(1);
    }
    sleep(1);
    close(fd);
    pthread_exit(NULL);
}





static unsigned short int getModbusCRC16(unsigned char *_pBuf, unsigned short int _usLen)
{
    unsigned short int CRCValue = 0xFFFF;           //初始化CRC变量各位为1
    unsigned char i,j;

    for(i=0;i<_usLen;++i)
    {
        CRCValue  ^= *(_pBuf+i);                    //当前数据异或CRC低字节
        for(j=0;j<8;++j)                            //一个字节重复右移8次
        {
            if((CRCValue & 0x01) == 0x01)           //判断右移前最低位是否为1
            {
                 CRCValue = (CRCValue >> 1)^0xA001; //如果为1则右移并异或表达式
            }else 
            {
                CRCValue >>= 1;                     //否则直接右移一位
            }           
        }
    } 
    return CRCValue;            
}