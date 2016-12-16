unsigned intermediary_sel[] = { /*lsb*/ 25, 26, 27 /*msb*/ }; // Select pins for intermediary multiplexers
unsigned output_sel[] = { /*lsb*/ 22, 23, 24 /*msb*/ }; // Select pins to choose ouput of which multiplexer
unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned input_voltage = 5.0;
float voltages[10][19]; // Values at each node

void setup(){
  Serial.begin(9600);  // serial comms for troubleshooting (always)
  for(unsigned pin = 0; pin < 3; pin++) // setup select pins
  {
    pinMode(intermediary_sel[pin], OUTPUT);
    pinMode(output_sel[pin], OUTPUT);
  } 


  


}


void loop()
{

  

unsigned short lines = 2;

  // For each line
  for (unsigned line = 0; line < lines; line++)
  {
      // For each muliplexer
     for (unsigned i = 0; i < 3; i++)
     {
        // Set the select of the output mux to the desired intermediary output
        digitalWrite(output_sel[0], (line*3 + i) & 0x1);
        digitalWrite(output_sel[1], (line*3 + i) & 0x2);
        digitalWrite(output_sel[2], (line*3 + i) & 0x4);
                
        // For each input to the intermediary multiplexer
        for (unsigned short j = 0; j < 8 && (i*8 + j) < 19; j++) // Room for 24 reads per intermediary set but only using 19 of them
        { 
          // Set select to that input
          digitalWrite(intermediary_sel[0], j & 0x1);
          digitalWrite(intermediary_sel[1], j & 0x2);
          digitalWrite(intermediary_sel[2], j & 0x4);


          voltages[line][(i*8 + j)] = analogRead(output) * (input_voltage / 1023.0);


        }  
      }     
    }

        
      

   


    for (unsigned line = 0; line < lines; line++)
    {
      Serial.print("Line ");
      Serial.print(line);
      Serial.print(":\t ");
      
      for (unsigned i = 0; i < 19; i++)
      {
          Serial.print(voltages[line][i]);
          Serial.print("\t");
      }
      
      Serial.print ("\n");
    }
    Serial.print ("\n");

//  // Testing pins
//   digitalWrite(output_sel[0], 0);
//   digitalWrite(output_sel[1], 1);
//   digitalWrite(output_sel[2], 0);
//   digitalWrite(intermediary_sel[0], 0);
//   digitalWrite(intermediary_sel[1], 1);
//   digitalWrite(intermediary_sel[2], 0);
//   
//   Serial.print(analogRead(output) * (input_voltage / 1023.0));
//   Serial.print ("\n");
  
 
    
  
}
