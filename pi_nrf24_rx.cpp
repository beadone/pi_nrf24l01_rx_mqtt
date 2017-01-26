/*
TMRh20 2014 - Updated to work with optimized RF24 Arduino library
*/


/**
 * Example for efficient call-response using ack-payloads
 *
 * This example continues to make use of all the normal functionality of the radios including
 * the auto-ack and auto-retry features, but allows ack-payloads to be written optionlly as well.
 * This allows very fast call-response communication, with the responding radio never having to
 * switch out of Primary Receiver mode to send back a payload, but having the option to switch to
 * primary transmitter if wanting to initiate communication instead of respond to a commmunication.
 */

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <RF24/RF24.h>


using namespace std;



static const char 					topic_backyardtemp1[] = "backyard/temp1";
static const char                                       topic_housetemp1[] = "house/temp1";
static const char                                       topic_househum1[] = "house/hum1";
static const char                                       topic_backyardhum1[] = "backyard/hum1";

// Hardware configuration
// Configure the appropriate pins for your connections

/****************** Raspberry Pi ***********************/

RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);



/********** User Config *********/
// Assign a unique identifier for this node, 0 or 1. Arduino example uses radioNumber 0 by default.
bool radioNumber = 1;

/********************************/


// Radio pipe addresses for the nodes to communicate.
//const uint8_t addresses[][6] = {"1Node","2Node","3Node","4Node","5Node","6Node"};
//const uint8_t addresses[][6] = {"1Node","2Node"};
const uint64_t addresses[6] = {0xF0F0F0F0E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};
//uint64_t addresses[][6] = {0xF0F0F0F0E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};
//const uint8_t addresses[][6] = {"1Node","2Node"};
//const uint8_t addresses[6] = {0xF0F0F0F0E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
bool role_ping_out = 1, role_pong_back = 0, role = 0;
//uint8_t counter = 1;                                // A single byte to keep track of the data being sent back and forth


uint8_t data[32];
unsigned long startTime, stopTime, counter, rxTimer=0;


int main(int argc, char** argv){

int i;
int j;
char temp[]="00.0";
char hum[]="00.0";

  cout << "RPi/RF24/examples/gettingstarted_call_response\n";
  radio.begin();
  radio.enableAckPayload();               // Allow optional ack payloads
  radio.enableDynamicPayloads();
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging

//const uint8_t addresses[][6] = {"1Node","2Node","3Node","4Node"}

      	radio.openWritingPipe(addresses[1]);
      	radio.openReadingPipe(1,addresses[0]);
      	radio.openReadingPipe(2,addresses[2]);  // open a second sender up
	radio.openReadingPipe(3,addresses[3]);
	radio.openReadingPipe(4,addresses[4]);
	radio.openReadingPipe(5,addresses[5]);
	radio.startListening();
	radio.writeAckPayload(1,&counter,1);


// forever loop
while (1){
// may reenable the ack to improve comms later
     // radio.writeAckPayload(pipeNo,&gotByte, 1 );   // This can be commented out to send empty payloads.	  

uint8_t pipeNo;
//bool result;
char buffer [50];  // holds the mqtt message
//printf("in pong loop \n\r");
     while(radio.available(&pipeNo)){
      radio.read(&data,32);
      counter++;
	printf("Data received %s  pipe %d \n\r", data,pipeNo);


	if (pipeNo==1){

		for (i=0;i<4;i++){
			temp[i] = data[i];
		} //for i

		for(j=0;j<4;j++){
			hum[j] = data[j+6];
		
		}	 //for j
	//	printf("pipe 1 backyard %s  Hum  %s \n\r", temp, hum);
		//printf("mosquitto_pub -d  -t backyard/temp1 -h 192.168.0.3 -m \"%s\"\n\r", temp);
		sprintf(buffer, "mosquitto_pub  -t backyard/temp1 -h 192.168.0.3 -m \"%s\"", temp);
		system(buffer);
		sprintf(buffer, "mosquitto_pub  -t backyard/humidity1 -h 192.168.0.3 -m \"%s\"", hum);
		//printf("mosquitto_pub -d  -t backyard/hum1 -h 192.168.0.3 -m \"%s\"\n\r", hum);
		system(buffer);


	}

        if (pipeNo==2){

		for (i=0;i<4;i++){
			temp[i] = data[i];
		} //for i

		for(j=0;j<4;j++){
			hum[j] = data[j+6];
		
		} //for j
	//	printf("pipe 2 house  %s  Hum  %s \n\r", temp, hum);
		sprintf(buffer, "mosquitto_pub  -t house/temp1 -h 192.168.0.3 -m \"%s\"", temp);
		//printf("mosquitto_pub  -t house/temp1 -h 192.168.0.3 -m \"%s\"\n\r", temp);
		system(buffer);
		//printf("mosquitto_pub -d  -t house/hum1 -h 192.168.0.3 -m \"%s\"\n\r", hum);
		sprintf(buffer, "mosquitto_pub  -t house/humidity1 -h 192.168.0.3 -m \"%s\"", hum);
		system(buffer);


        }

        if (pipeNo==3){

		for (i=0;i<4;i++){
			temp[i] = data[i];
		} //for i

		for(j=0;j<4;j++){
			hum[j] = data[j+6];
		
		} //for j
		//printf("pipe 3 stm32 data  %s \n\r", data);
		//printf("pipe 3   %s  Hum  %s \n\r", temp, hum);
			//printf("pipe 3 stm32  %s  Hum  \n\r", data);
		sprintf(buffer, "mosquitto_pub   -t house/temp2 -h 192.168.0.3 -m \"%s\"", temp);
		//printf("mosquitto_pub  -t house/temp2 -h 192.168.0.3 -m \"%s\"\n\r", temp);
		system(buffer);
		//printf("mosquitto_pub -d  -t house/hum2 -h 192.168.0.3 -m \"%s\"\n\r", hum);
		sprintf(buffer, "mosquitto_pub  -t house/humidity2 -h 192.168.0.3 -m \"%s\"", hum);
		system(buffer);


        }

        if (pipeNo==182){

		for (i=0;i<4;i++){
			temp[i] = data[i];
		} //for i

		for(j=0;j<4;j++){
			hum[j] = data[j+6];
		
		} //for j
		//printf("pipe 182 stm32  %s  Hum  %s \n\r", temp, hum);
			printf("pipe 182 stm32  %s  Hum  \n\r", data);
		sprintf(buffer, "mosquitto_pub  -t house/temp2 -h 192.168.0.3 -m \"%s\"", temp);
		//printf("mosquitto_pub  -t house/temp2 -h 192.168.0.3 -m \"%s\"\n\r", temp);
		system(buffer);
		//printf("mosquitto_pub -d  -t house/hum2 -h 192.168.0.3 -m \"%s\"\n\r", hum);
		sprintf(buffer, "mosquitto_pub -t house/humidity2 -h 192.168.0.3 -m \"%s\"", hum);
		system(buffer);


        }

     }
//delay(12000);
delay(2000);
//radio.printDetails();

/**
   //if(millis() - rxTimer > 100000){
   if(millis() - rxTimer > 1000){
	if (counter > 0) {

     		rxTimer = millis();
     		printf("Rate: ");
     		float numBytes = counter*32;
     		printf("%.2f KB/s \n\r",numBytes/1000);
     		printf("Payload Count: %lu \n\r", counter);
     		printf("Data received %s  pipe %d \n\r", data,pipeNo);

     		counter = 0;
	}



	//delay(10000);

   }
*/




} //while 1
} //main


