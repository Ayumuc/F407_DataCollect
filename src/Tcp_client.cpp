/*
Author:LZH
TARGET:TCP客户端获取RS232信息然后将获取的信息保存起来，让TCP服务端调用
*/
#include "../inc/main.h"
#include "../inc/Acceleration.h"

void Accleration_decode(uint8_t* buf);
void* Tcp_client_init(void* arg){
    int ret = 0;
    uint8_t Tcp_client_buffer[11] = {0};
//  创建一个通信的套接字，需要指定服务器的IP和端口号
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd != -1);

    struct sockaddr_in ser_addr;
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port =htons(8000);//转化端口号
    ser_addr.sin_addr.s_addr = inet_addr("192.168.1.188");//回环地址


//连接服务器，需要知道服务器绑定的IP和端口
    int res = connect(sockfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    if(!(res > 0)){
        printf("connect 192.168.1.188 success!\n");
    }
    printf("res:%d\n",res);
    while(1){
        if(recv(sockfd,Tcp_client_callbackbuf,127,0) < 0){
             perror("recv error!\n");
        }
#if PRINT
        else{
            printf("Data from sever:\n");
            for(int i = 0 ; i < sizeof(Tcp_client_callbackbuf) ; i++){
            printf("%02x ",Tcp_client_callbackbuf[i]);
            }
            printf("\n");
        })
#endif
    for(int i = 0 ; i < (sizeof(Tcp_client_callbackbuf) - 11) ; i++){
        if(Tcp_client_callbackbuf[i] == 0x55 && Tcp_client_callbackbuf[i + 1] == 0x51){
            for(int j = 0 ; j < 11 ; j++ ){
                Tcp_client_buffer[j] = Tcp_client_callbackbuf[i+j]; 
            }
#if PRINT
            printf("Tcp_client_buffer:\n");
            for(int i = 0 ; i < sizeof(Tcp_client_buffer) ; i++){
            printf("%02x ",Tcp_client_buffer[i]);
            }
            printf("\n");
#endif
            Accleration_decode(Tcp_client_buffer);
        }
    }
    }
    pthread_exit(NULL);
}
/*00 00 00 00 de 0c 91
 55 78 e0 df 0c cf 55 54
  00 00 00 00 00 00 00 00 a9 
  55 51 0b 00 05 00 38 08 df 0c e1 
  55 52 00 00 00 00 00 00 df 0c 92 
  55 53 1a 00 cb ff 78 e0 df 0c cf 
  55 54 00 00 00 00 00 00 00 00 a9 
  55 51 0b 00 05 00 38 08 df 0c e1 
  55 52 00 00 00 00 00 00 df 0c 92 
  55 53 1a 00 cb ff 78 e0 df 0c cf 
  55 54 00 00 00 00 00 00 00 00 a9 
  55 51 0a 00 05 00 32 08 de 0c d9 
  55 52 00 00 00*/

/*SUMCRC(数据和校验)：
SUMCRC=0x55+TYPE+DATA1L+DATA1H+DATA2L+DATA2H+DATA3L+DATA3H+DATA4L+DATA4H
SUMCRC为char型，取校验和的低8位*/

/*  TYPE	备注
    0x50	时间
    0x51	加速度
    0x52	角速度
    0x53	角度
    0x54	磁场
    0x55	端口状态
    0x56	气压高度
    0x57	经纬度
    0x58	地速
    0x59	四元数
    0x5A	GPS定位精度
    0x5F	读取 
    55 51 0b 00 05 00 38 08 df 0c e1 
    */
#define g 9.8
void Accleration_decode(uint8_t* buf){
    uint8_t sum = 0;
    Aer.Acler_head = buf[0];
    Aer.Type = buf[1];
//    printf("buf[3]:%02x,buf[2]:%02x\n",buf[3],buf[2]);
    Aer.Ax = ((float)(((short)(buf[3] << 8) | (short)(buf[2]))*16)/32768);
    Aer.Ay = ((float)(((short)(buf[5] << 8) | (short)(buf[4]))*16)/32768);
    Aer.Az = ((float)(((short)(buf[7] << 8) | (short)(buf[6]))*16)/32768);
    Aer.TL = buf[8];
    Aer.Th = buf[9];
    for(int i = 0 ; i < 10 ; i++){
        Aer.Acler_sum = Aer.Acler_sum + buf[i];
    }
#if PRINT
    printf("Ax:%f\n Ay:%f\n Az:%f\n",Aer.Ax,Aer.Ay,Aer.Az);
#endif
}