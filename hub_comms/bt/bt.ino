#include <SoftwareSerial.h>
 
SoftwareSerial bt(10, 11); // RX, TX
 
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 bt.begin(9600);
  Serial.println("Ready!");
 
  // set the data rate for the SoftwareSerial port
 
  // for HC-05 use 38400 when poerwing with KEY/STATE set to HIGH on power on
  
}
 
void loop() // run over and over
{
  if (bt.available())
  {
    Serial.write(bt.read());
  }
  if (Serial.available())
  {
    bt.write(Serial.read());
  }
}



