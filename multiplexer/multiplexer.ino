unsigned intermediary_sel[] = { 25, 26, 27 }; // Select pins for intermediary multiplexers
unsigned output_sel[] = { 22, 23, 24 }; // Select pins to choose ouput of which multiplexer
unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned input_voltage = 5.0;


void setup(){
  Serial.begin(9600);  // serial comms for troubleshooting (always)
  for(int pin = 0; pin < 3; pin++) // setup select pins
  {
    pinMode(intermediary_sel[pin], OUTPUT);
    pinMode(output_sel[pin], OUTPUT);

    digitalWrite(output_sel[2], LOW);

  } 

}


void loop()
{
    // For each muliplexer
   for (unsigned short i = 0; i < 3; i++)
   {
      digitalWrite(output_sel[0], i & 0x1);
      digitalWrite(output_sel[1], i & 0x2);
      
      // For each input to the multiplexer
      for (unsigned short j = 0; j < 8; j++)
      { 
        digitalWrite(intermediary_sel[0], j & 0x1);
        digitalWrite(intermediary_sel[1], j & 0x2);
        digitalWrite(intermediary_sel[2], j & 0x4);

        Serial.print(analogRead(output) * (input_voltage / 1023.0));
        Serial.print ("\t");

        
      }


      
   }

   
   Serial.print ("\n");
  
 
    
  
}
