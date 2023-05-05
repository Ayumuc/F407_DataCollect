/*
Author:LZH
TARGET:RS485解析和组包和初始化
*/

#include<stdio.h>  
#include<stdlib.h>  
#include<unistd.h>  
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>  
#include<termios.h>  
#include<errno.h>  
#include"../inc/main.h"
uint8_t usart_buf[1024];
#define FALSE -1  
#define TRUE 0  
// 0x01 获取设备信息 上位机→SKP40
// 0x03 设置测量频率 上位机→SKP40
// 0x04 设置数据格式 上位机→SKP40
// 0x0D 设置测量模式 上位机→SKP40
// 0x05 启动测量 上位机→SKP40
// 0x06 停止测量 上位机→SKP40
// 0x07 测量数据返回 SKP40→上位机
// 0x0E 高速测量数据返回 测距雷达→上位机
// 0x08 保存设置 上位机→SKP40
// 0x0A 获取序列号 上位机→SKP4
#define GET_MSG 0X01
#define SET_CHECK_FUE 0X03
#define SET_DATA_FORMAT 0X04
#define SET_CHECK_MODLE 0X0D
#define START_CHECK 0X05
#define STOP_CHECK 0X06
#define CHECK_DATA_CALLBACK 0X07
#define FAST_CALLBACK 0X0E
#define SAVE_SET 0X08
#define GET_NUM 0X0A
#define TOF 0
#define speed_radio 1

void* Usart_init (void*arg);
uint8_t crc_high_first(uint8_t *ptr, uint8_t len);
int Usart_encode(uint8_t* buf);
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,B38400, B19200, B9600, B4800, B2400, B1200, B300, };  
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400, 19200,  9600, 4800, 2400, 1200,  300, };  
extern int data_pos;
extern char speed_value[4];
void set_speed(int fd, int speed){  
  int   i;   
  int   status;   
  struct termios   Opt;  
  tcgetattr(fd, &Opt);   
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {   
    if  (speed == name_arr[i]) {       
      tcflush(fd, TCIOFLUSH);       
      cfsetispeed(&Opt, speed_arr[i]);    
      cfsetospeed(&Opt, speed_arr[i]);     
      status = tcsetattr(fd, TCSANOW, &Opt);    
      if  (status != 0) {          
        perror("tcsetattr fd1");    
        return;       
      }      
      tcflush(fd,TCIOFLUSH);     
    }    
  }  
}  
  
int set_Parity(int fd,int databits,int stopbits,int parity)  
{   
    struct termios options;   
    if  ( tcgetattr( fd,&options)  !=  0) {   
        perror("SetupSerial 1");       
        return(FALSE);    
    }  
    options.c_cflag &= ~CSIZE;   
    switch (databits)   
    {     
    case 7:       
        options.c_cflag |= CS7;   
        break;  
    case 8:       
        options.c_cflag |= CS8;  
        break;     
    default:      
        fprintf(stderr,"Unsupported data size\n"); return (FALSE);    
    }  
    switch (parity)   
    {     
        case 'n':  
        case 'N':      
            options.c_cflag &= ~PARENB;   /* Clear parity enable */  
            options.c_iflag &= ~INPCK;     /* Enable parity checking */   
            break;    
        case 'o':     
        case 'O':       
            options.c_cflag |= (PARODD | PARENB);   
            options.c_iflag |= INPCK;             /* Disnable parity checking */   
            break;    
        case 'e':    
        case 'E':     
            options.c_cflag |= PARENB;     /* Enable parity */      
            options.c_cflag &= ~PARODD;      
            options.c_iflag |= INPCK;       /* Disnable parity checking */  
            break;  
        case 'S':   
        case 's':  /*as no parity*/     
            options.c_cflag &= ~PARENB;  
            options.c_cflag &= ~CSTOPB;break;    
        default:     
            fprintf(stderr,"Unsupported parity\n");      
            return (FALSE);    
        }    
      
    switch (stopbits)  
    {     
        case 1:      
            options.c_cflag &= ~CSTOPB;    
            break;    
        case 2:      
            options.c_cflag |= CSTOPB;    
           break;  
        default:      
             fprintf(stderr,"Unsupported stop bits\n");    
             return (FALSE);   
    }   
    /* Set input parity option */   
    if (parity != 'n')     
        options.c_iflag |= INPCK;   
    tcflush(fd,TCIFLUSH);  
    options.c_cc[VTIME] = 150;   
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */  
    if (tcsetattr(fd,TCSANOW,&options) != 0)     
    {   
        perror("SetupSerial 3");     
        return (FALSE);    
    }   
    return (TRUE);    
}  

void* Usart_init (void*arg){
    int usart_fd = 0;
    int txt_fd = 0 ;
    char file_buf[30] = {0};
    bzero(speed_value,sizeof(speed_value));
    txt_fd = open("./usart_config.txt",O_RDWR);
    if(txt_fd < 0 ){
        printf("open txt false!\n");
        //pthread_exit(NULL);
    }
    read(txt_fd,file_buf,sizeof(file_buf));
    //printf("file_buf:%s\n",file_buf);
    close(txt_fd);
    usart_fd = open("/dev/ttysWK0",O_RDWR);
    if(usart_fd < 0 ){
        perror("open usart error!\n");
        //pthread_exit(NULL);
    }
    else{
        printf("open rs485 success!\n");
    }
    set_speed(usart_fd,9600);  
    if (set_Parity(usart_fd,8,1,'N') == FALSE)  {  
        printf("Set Parity Error\n");  
        exit (0);  
    }
    while(1){

        // if(write(usart_fd,"q",sizeof("q")) < 0);{
        //     perror("write error!\n");
        // }
        if(read(usart_fd,usart_buf,sizeof(usart_buf)) > 0 ){
            //printf("%s\n",usart_buf);
            //data_pos = Usart_encode(usart_buf);
#if speed_radio
        sprintf(speed_value,"%c%c%c",usart_buf[1],usart_buf[2],usart_buf[3]);
        //printf("%s\n",speed_value);
#endif
        }
    }
    close(usart_fd);
    pthread_exit(NULL);
}

int Usart_encode(uint8_t* buf){
#if TOF
    int pos;
    DATA data;
    data.data_head = buf[0];
    data.data_key = buf[1];
    data.data_value[0] = buf[2];
    data.data_value[1] = buf[3];
    data.data_value[2] = buf[4];
    data.data_value[3] = buf[5];
    data.data_crc8 = crc_high_first(buf,sizeof(buf));
    if(data.data_crc8 != buf[6]){
        printf("crc8 error!\n");
        return 0;
    }
    data.data_end = buf[7];
    if(data.data_head != 0x55){
        printf("帧头错误\n");
        return 0 ;
    }
    if(data.data_value[0] != 0x00){
        printf("system error!\n");
        return 0 ;
    }
    pos = data.data_value[1] << 8 | data.data_value[2];
    pos = pos << 8 | data.data_value[3];
    return pos;
#endif
        return 0;
}

uint8_t crc_high_first(uint8_t *ptr, uint8_t len){
    uint8_t i;
    uint8_t crc = 0x00;/*计算初始crc*/
    while(len--){
        crc ^= *ptr++;
        for(i = 8 ; i > 0 ; --i){
            if(crc & 0x80){
                crc = (crc << 1)^0x31;
            }
            else{
                crc = (crc << 1);
            }
        }
    }
    return crc;
}