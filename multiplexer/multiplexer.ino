unsigned intermediary_sel[] = { /*lsb*/ 25, 26, 27 /*msb*/ }; // Select pins for intermediary multiplexers
unsigned output_sel[] = { /*lsb*/ 22, 23, 24 /*msb*/ }; // Select pins to choose ouput of which multiplexer
unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned input_voltage = 5.0;
float voltages[10][18]; // Values at each node

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

unsigned short index;

  // For each line
  for (unsigned line = 0; line < 2; line++)
  {
    Serial.print("Line ");
    Serial.print(line);
    Serial.print(":\t");
    
      // For each muliplexer
     for (unsigned i = 0; i < 3; i++)
     {
        // Set the select of the output mux to the desired intermediary output
        digitalWrite(output_sel[0], (line*3 + i) & 0x1);
        digitalWrite(output_sel[1], (line*3 + i) & 0x2);
        digitalWrite(output_sel[2], (line*3 + i) & 0x4);
                
        // For each input to the intermediary multiplexer
        for (unsigned short j = 0; j < 8 && (i*8 + j) <= 18; j++)
        { 
          // Set select to that input
          digitalWrite(intermediary_sel[0], j & 0x1);
          digitalWrite(intermediary_sel[1], j & 0x2);
          digitalWrite(intermediary_sel[2], j & 0x4);

          // Room for 24 reads per intermediary set but only using 18 of them
          voltages[line][(i*8 + j)] = analogRead(output) * (input_voltage / 1023.0);
          Serial.print(voltages[line][(i*8 + j)]);
          Serial.print("\t");
        }  
      }
      
      Serial.print ("\n");      
    }

        
      

   

/*
    for (unsigned v = 0; v < 20; v++)
    {
      Serial.print(voltages[v]);
      Serial.print ("\t");
    }
*/   
   Serial.print ("\n");
  
 
    
  
}
