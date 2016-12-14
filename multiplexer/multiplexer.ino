unsigned intermediary_sel[] = { 25, 26, 27 }; // select pins for intermediary multiplexers
unsigned output_sel[] = { 22, 23, 24 };
unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned input_voltage = 5.0;


void setup(){
  Serial.begin(9600);  // serial comms for troubleshooting (always)
  for(int pin = 0; pin < 3; pin++) // setup select pins
  {
    pinMode(intermediary_sel[pin], OUTPUT);
    pinMode(output_sel[pin], OUTPUT);

  } 

}


void loop()
{
   for (unsigned short select = 0; select < 8; select++)
   {
      digitalWrite(sel[0], select & 0x1);
      digitalWrite(sel[1], select & 0x2);
      digitalWrite(sel[2], select & 0x4);

    


      Serial.print(analogRead(common) * (input_voltage / 1023.0));
      Serial.print ("\t");
   }

   
   Serial.print ("\n");
  
 
    
  
}
