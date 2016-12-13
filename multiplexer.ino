int sel[] = { 22, 23, 24 }; // select pins on 4051 (Digital 22, 23, 24)
int common = 0;   // common in/out pins (analog A0)
int input_voltage = 3.3;


void setup(){
  Serial.begin(9600);  // serial comms for troubleshooting (always)
  for(int pin = 0; pin < 3; pin++){ // setup select pins
    pinMode(sel[pin], OUTPUT);
  } 

}


void loop()
{
   for (unsigned short select = 0; select < 8; select++)
   {
      digitalWrite(sel[0], select & 0x1);
      digitalWrite(sel[1], select & 0x2);
      digitalWrite(sel[2], select & 0x4);

      Serial.print ("Select: ");
      Serial.print (select);
      Serial.print ("\t");
      Serial.print ("Voltage: ");
      Serial.println(analogRead(common) * (input_voltage / 1023.0));
   }

   
  
  
 
    
  
}



