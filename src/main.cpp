#include "../inc/main.h"
#include "../inc/Acceleration.h"
int data_pos;
char speed_value[4];
uint8_t Tcp_client_callbackbuf[128];
Acler Aer;
int main(int argc , char**argv){
    int id = 0;
    pthread_t thread_id;
    pthread_t usart_id;
    pthread_t Client_id;
    pthread_create(&Client_id,NULL,Tcp_client_init,NULL);   
    pthread_create(&usart_id,NULL,Usart_init,NULL);
    pthread_create(&thread_id,NULL,Tcpsever_init,NULL);
    while (1){
    
    }
}