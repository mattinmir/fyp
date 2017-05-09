#include <SPI.h>
#include <RF24.h>

// (ce, csn)
RF24 radio(9,10);


void setup()
{
	while(!Serial); // Loop until serial connection is available
	
	Serial.begin(9600);
	
	
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(0x76);
	radio.openWritingPipe(0xF0F0F0F0E1LL);
	radio.openReadingPipe(1, 0xE8E8F0F0E1LL);
	
	
	radio.enableDynamicPayloads();
	radio.powerUp();
}

void loop()
{
	radio.startListening();
	Serial.println("Radio on");
	
	char recievedMessage[32] = {0};
	if (radio.available())
	{
		radio.read(recievedMessage, sizeof(recievedMessage));
		Serial.print("Recieved: ");
		Serial.println(recievedMessage);
		
		radio.stopListening();
		Serial.println("Radio off");

		String stringMessage(recievedMessage);
		
		if (stringMessage == "COMMAND")
		{
			Serial.println("Message matches command");
			const char text[] = "Hello, World!";
			radio.write(text, sizeof(text));
			Serial.println("Sent Response");
		}
	}
	
	
	delay(100);
}