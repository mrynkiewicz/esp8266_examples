#include <Wire.h>

#define MY_ADDR   4

int LED = 13;
char rx_buffer[32] ;
uint32_t cnt =0;
uint8_t message[50];
void setup() {

  // Setup serial port speed
  Serial.begin(115200);
  // Define the LED pin as Output
  pinMode (LED, OUTPUT);
  
 // Start the I2C Bus as Slave
  Wire.begin(MY_ADDR); 
  
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);

  sprintf(message,"Slave is ready : Address 0x%x",MY_ADDR);
  Serial.println((char*)message );  
  Serial.println("Waiting for data from master");  
  digitalWrite(LED, HIGH);
}

void loop(void)
{
  
}

void receiveEvent(int bytes) 
{
 while( Wire.available() )
 {
   rx_buffer[cnt++] = Wire.read();
 }
  rx_buffer[cnt] = '\0';
  cnt=0;
  Serial.print("Received:");  
  Serial.println((char*)rx_buffer);  
}
