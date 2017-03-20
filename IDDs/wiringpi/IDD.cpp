#include <wiringPi.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>

#include "LIS3DH.h"


#define LED 25
#define BTN 1

int change_mode(int status){
if(status == 1){
digitalWrite(LED, 0);
delay(200);
puts("Status :off");
return 0;
}
if(status == 0){
digitalWrite(LED, 1);
delay(200);
puts("Status :on");
return 1;
}
return 0;
}

int setup_acc(int fd){

fd = wiringPiI2CSetup(0x18);

  wiringPiI2CWriteReg8(fd, LIS3DH_REG_CTRL1, 0x07);

  // 400Hz rate
uint8_t ctl1 =  wiringPiI2CReadReg8(fd, LIS3DH_REG_CTRL1);
  ctl1 &= ~(0xF0); // mask off bits
  ctl1 |= (LIS3DH_DATARATE_400_HZ  << 4);

   wiringPiI2CWriteReg8(fd, LIS3DH_REG_CTRL1, ctl1);

  // High res & BDU enabled
   wiringPiI2CWriteReg8(fd, LIS3DH_REG_CTRL4, 0x88);

  // DRDY on INT1
   wiringPiI2CWriteReg8(fd, LIS3DH_REG_CTRL3, 0x10);

  // enable adcs
   wiringPiI2CWriteReg8(fd, LIS3DH_REG_TEMPCFG, 0x80);

  //set range
     uint8_t r = wiringPiI2CReadReg8(fd, LIS3DH_REG_CTRL4);
  r &= ~(0x30);
  r |=  LIS3DH_RANGE_16_G << 4;
  wiringPiI2CWriteReg8(fd, LIS3DH_REG_CTRL4, r);

return fd;
}

void read_acc(int fd){

int16_t x = wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_X_L);
int16_t y = wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Y_L);
int16_t z = wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Z_L);

x |= ((uint16_t)wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_X_L))<<8;
y |= ((uint16_t)wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Y_L))<<8;
z |= ((uint16_t)wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Z_L))<<8;

int8_t x_1 = wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_X_H);
int8_t y_1 = wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Y_H);
int8_t z_1 = wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Z_H);

x_1 |= ((uint16_t)wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_X_H))<<8;
y_1 |= ((uint16_t)wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Y_H))<<8;
z_1 |= ((uint16_t)wiringPiI2CReadReg8(fd, LIS3DH_REG_OUT_Z_H))<<8;

printf("x: %d, y: %d, z: %d\n", (x_1)+1, (y_1)+1, (z_1)-4);
}

int main (void) {

    wiringPiSetup ();
    pinMode (LED, OUTPUT);
    pinMode (BTN, INPUT);


	digitalWrite(LED, 0);

int status = 0;

int fd = 0;

fd = setup_acc(fd);
printf("device ready");

   while(1)
    {
	if(status == 1){
delay(900);
         read_acc(fd);
}

	if(digitalRead(BTN) == LOW){
	status = change_mode(status);
	}



}
     
     return 0;
}
