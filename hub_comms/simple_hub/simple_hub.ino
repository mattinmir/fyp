#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

#include <SPI.h>

// (ce, csn)
RF24 radio(7,8);


void setup()
{
  Serial.begin(9600);
  printf_begin();
  //Serial.print("Connected: ");
  //Serial.print(radio.isChipConnected());
  
  radio.begin();
  radio.printDetails();
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);

  radio.enableDynamicPayloads();
  radio.powerUp();
}

void loop()
{
  
  const char text[] = "Hello World!";

  

 Serial.print("Write: ");
 Serial.println(radio.write(&text, sizeof(text)));
 Serial.println(radio.failureDetected);  
  
  
  delay(1000);
}
