/* DHT11 temperature sensor library
   Usage:
   		Set DHT PIN using  setDHTPin(pin) command
   		getFtemp(); this returns temperature in F
   Sam Johnston
   October 2016
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "dht11.h"




#include <inttypes.h>




int humidity = 0;
int temperature = 0;
int Ftemperature = 0;

uint8_t bits[5];
uint32_t _lastreadtime=-2000;

int DHT_DATA[3] = {0,0,0};
int DHT_PIN = 4;

void setDHTPin(int PIN)
{
	DHT_PIN = PIN;
	gpio_pad_select_gpio(DHT_PIN);
	gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(DHT_PIN, GPIO_PULLUP_ONLY);
}

void errorHandle(int response)
{
	switch(response) {

		case DHT_TIMEOUT_ERROR :
			printf("DHT Sensor Timeout!\n");
		case DHT_CHECKSUM_ERROR:
			printf("CheckSum error!\n");
		case DHT_OKAY:
			break;
		default :
			printf("Dont know how you got here!\n");
	}
	temperature = 0;
	humidity = 0;

}

void sendStart()
{
	gpio_set_level(DHT_PIN, 1);
	ets_delay_us(250000);
  //Send start signal from ESP32 to DHT device
  gpio_set_direction(DHT_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(DHT_PIN,0);
  //vTaskDelay(36 / portTICK_RATE_MS);
  ets_delay_us(20000);

  gpio_set_level(DHT_PIN,1);
  ets_delay_us(40);

  gpio_set_direction(DHT_PIN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(DHT_PIN, GPIO_PULLUP_ONLY);
	ets_delay_us(10);

}

int getData()
{

	uint32_t currenttime = esp_timer_get_time()/1000;
	if ((currenttime - _lastreadtime) < 2000) {
    return DHT_OKAY; // return last correct measurement
  }
  _lastreadtime = currenttime;
  //Variables used in this function
  int counter = 0;
  //uint8_t byteCounter = 0;
  //uint8_t cnt = 7;

  for (int i = 0; i <5; i++)
  {
  	bits[i] = 0;
  }

	uint32_t cycles[80];

  sendStart();


  //Now that the DHT has pulled the line low,
  //it will keep the line low for 80 us and then high for 80us
  //check to see if it keeps low
  counter = 0;
  while(gpio_get_level(DHT_PIN)==0)
  {
  	if(counter++ > 80)
  	{
            return DHT_TIMEOUT_ERROR;
  	}
  	ets_delay_us(1);
  }

  counter = 0;
  while(gpio_get_level(DHT_PIN)==1)
  {
  	if(counter++ > 80)
  	{
            return DHT_TIMEOUT_ERROR;
  	}
  	ets_delay_us(1);
  }
  // If no errors have occurred, it is time to read data
  //output data from the DHT11 is 40 bits.
  //Loop here until 40 bits have been read or a timeout occurs

  for(int i = 0; i < 80; i+=2)
  {
      //int currentBit = 0;
      //starts new data transmission with 50us low signal
      counter = 0;
      while(gpio_get_level(DHT_PIN)==0)
  	  {
  	  	if (counter++ > 55)
  	  	{
            return DHT_TIMEOUT_ERROR;
  	  	}
  	  	ets_delay_us(1);
  	  }

			cycles[i] = counter;

  	  //Now check to see if new data is a 0 or a 1
      counter = 0;
      while(gpio_get_level(DHT_PIN)==1)
  	  {
  	  	if (counter++ > 75)
  	  	{
            return DHT_TIMEOUT_ERROR;
  	  	}
  	  	ets_delay_us(1);
  	  }
			cycles[i+1] = counter;
  }

	//add the current reading to the output data
	//since all bits where set to 0 at the start of the loop, only looking for 1s
	//look for when count is greater than 40 - this allows for some margin of error

	for (int i=0; i<40; ++i) {
		uint32_t lowCycles = cycles[2*i];
		uint32_t highCycles = cycles[2*i+1];
		bits[i/8] <<= 1;
		// Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      bits[i/8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
	}

  if (bits[4] != ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF))
  {
  	return DHT_CHECKSUM_ERROR;
  }

  return DHT_OKAY;
}

int getTemp()
{
	int Data = getData();
	if (Data == 0){
		return bits[2];
	}
	return Data;
}

int getHumidity()
{
	int Data = getData();
	if (Data == 0){
		return bits[0];
	 }
	return Data;
}
