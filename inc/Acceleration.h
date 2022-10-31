#ifndef __ACCELERATION_H
#define __ACCELERATION_H

#include "main.h"
typedef struct Acceleration{
    uint8_t Acler_head;
    uint8_t Type;
    float Ax;
    float Ay;
    float Az;
    uint8_t TL;
    uint8_t Th;
    uint8_t Acler_sum;
}Acler;

extern Acler Aer;


/*名称	描述	备注
  AxL	加速度X低8位	加速度X=((AxH<<8)|AxL)/32768*16g
(g为重力加速度，可取9.8m/s2)
  AxH	加速度X高8位
  AyL	加速度Y低8位	加速度Y=((AyH<<8)|AyL)/32768*16g
(g为重力加速度，可取9.8m/s2)
  AyH	加速度Y高8位
  AzL	加速度Z低8位	加速度Z=((AzH<<8)|AzL)/32768*16g
(g为重力加速度，可取9.8m/s2)
  AzH	加速度Z高8位
  TL	温度低8位	温度计算公式：
温度=((TH<<8)|TL) /100 ℃
  TH	温度高8位
  SUM	校验和	SUM=0x55+0x51+AxL+AxH+AyL+AyH+AzL+AzH+TL+Th*/

#endif /* __ACCELERATION_H */