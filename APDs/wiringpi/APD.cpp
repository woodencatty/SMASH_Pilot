#include "2591a.h" //header including memory information

#include <wiringPi.h>
#include <stdio.h>
#include <softTone.h>    //piezo lib.
#include <stdlib.h>      //used in handling sensor data
#include <stdint.h>      //used in handling sensor data
#include <wiringPiI2C.h> //Wiringpi i2c lib
#include <unistd.h>      //used sensor data storage

//pin number definition
#define PIEZO 1

#define LBTN 4
#define RBTN 5
#define ABTN 2

#define RLED 25
#define GLED 24
#define BLED 22

#define DHT_PIN 0

#define MAX_TIMINGS 200 //temp sensor timing

int data[5] = {0, 0, 0, 0, 0}; //temp data array

void read_dht_data()
{
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(10);

    /* prepare to read the pin */
    pinMode(DHT_PIN, INPUT);

    /* detect change and read data */
    for (i = 0; i < MAX_TIMINGS; i++)
    {
	counter = 0;
	while (digitalRead(DHT_PIN) == laststate)
	{
	    counter++;
	    delayMicroseconds(1);
	    if (counter == 255)
	    {
		break;
	    }
	}
	laststate = digitalRead(DHT_PIN);

	if (counter == 255)
	    break;

	/* ignore first 3 transitions */
	if ((i >= 4) && (i % 2 == 0))
	{
	    /* shove each bit into the storage bytes */
	    data[j / 8] <<= 1;
	    if (counter > 16)
		data[j / 8] |= 1;
	    j++;
	}
    }

    /*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
    if ((j >= 40) &&
	(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
    {
	float h = (float)((data[0] << 8) + data[1]) / 10;
	if (h > 100)
	{
	    h = data[0]; // for DHT11
	}
	float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
	if (c > 125)
	{
	    c = data[2]; // for DHT11
	}
	if (data[2] & 0x80)
	{
	    c = -c;
	}
	float f = c * 1.8f + 32;
	printf("Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f);
    }
}

#define TSL2561_ADDR_LOW (0x29)
#define TSL2561_ADDR_FLOAT (0x39)
#define TSL2561_ADDR_HIGH (0x49)

void getLux(int fd)
{
    Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

    uint16_t ch0, ch1;
    uint32_t visible_and_ir, lux;

    visible_and_ir = tsl.Adafruit_TSL2591::getFullLuminosity(fd);

    visible_and_ir = tsl.Adafruit_TSL2591::getFullLuminosity(fd);

    // Reads two byte value from channel 0 (visible + infrared)
    ch0 = (visible_and_ir & 0xFFFF);
    // Reads two byte value from channel 1 (infrared)
    ch1 = (visible_and_ir >> 16);

    lux = tsl.Adafruit_TSL2591::calculateLux(ch0, ch1);

    //    printf("TSL2591 Full Spectrum: %zu\n",ch0);
    //    printf("TSL2591 Infrared: %zu\n",ch1);
    //   if ( ch0 >= ch1 )  printf("TSL2591 Visible: %zu\n",ch0-ch1);

    if (lux < 20)
    {
	digitalWrite(BLED, 0);
    }
    else
    {
	digitalWrite(BLED, 1);
    }
} //output : LED  // normal = 40 ~ 200

int change_mode(int status)
{ // toggle status
    if (status == 1)
    {
	digitalWrite(GLED, 1);
	printf("de-activate\n");
	return 0;
    }
    if (status == 0)
    {
	digitalWrite(GLED, 0);
	printf("activate\n");
	return 1;
    }
    printf("!!!!!!!!!!!!!error!!!!!!!!!\n\n");
    return 0;
}

void pushbutton_left()
{ // left button : beep 1sec
    softToneCreate(PIEZO);
    softToneWrite(PIEZO, 400);
    delay(400);
    softToneWrite(PIEZO, 0);
}

void pushbutton_right()
{ // right button : beep 2sec
    softToneCreate(PIEZO);
    softToneWrite(PIEZO, 400);
    delay(400);
    softToneWrite(PIEZO, 800);
    delay(400);
    softToneWrite(PIEZO, 0);
}

int main()
{
    //Initializing
    wiringPiSetup();
    pinMode(PIEZO, OUTPUT);
    pinMode(DHT_PIN, INPUT);

    pinMode(LBTN, INPUT);
    pinMode(RBTN, INPUT);
    pinMode(ABTN, INPUT);

    pinMode(RLED, OUTPUT);
    pinMode(GLED, OUTPUT);
    pinMode(BLED, OUTPUT);

    digitalWrite(RLED, 1);
    digitalWrite(GLED, 1);
    digitalWrite(BLED, 1);

    int fd = wiringPiI2CSetup(0x29);

    printf("Initialized!\n\n");

    int count = 0;
    int status = 0;

    while (1)	// loop
    {
		//button handling
	if (digitalRead(ABTN) == LOW)
	{
	    status = change_mode(status);
	    delay(200);
	}

	if (status == 1)
	{
	    if (digitalRead(LBTN) == LOW)
	    {
		pushbutton_left();
	    }
	    if (digitalRead(RBTN) == LOW)
	    {
		pushbutton_right();
	    }
	}

	count++;

	if (count > 15)	// light sensor output : LED
	{
	    read_dht_data();
	    count = 0;
	}
	getLux(fd);
    }
    return 0;
}
