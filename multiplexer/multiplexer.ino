#define nodes_per_line 19



unsigned intermediary_sel[] = { /*lsb*/ 25, 26, 27 /*msb*/ }; // Select pins for intermediary multiplexers
unsigned output_sel[] = { /*lsb*/ 22, 23, 24 /*msb*/ }; // Select pins to choose ouput of which multiplexer
unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned input_voltage = 5.0;
unsigned lines = 2;
unsigned counter = 0;
float voltages[10][nodes_per_line]; // Values at each node [No of lines][Nodes per line]

typedef unsigned rod_view_t[10][nodes_per_line]; // Holds representation of board in rod format 


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
        for (unsigned short j = 0; j < 8 && (i*8 + j) < nodes_per_line; j++) // Room for 24 reads per intermediary set but only using 19 of them
        { 
          // Set select to that input
          digitalWrite(intermediary_sel[0], j & 0x1);
          digitalWrite(intermediary_sel[1], j & 0x2);
          digitalWrite(intermediary_sel[2], j & 0x4);


          voltages[line][(i*8 + j)] = analogRead(output) * (input_voltage / 1023.0);


        }  
      }     
    }



    

/* Find connections */
  for (unsigned line = 0; line < lines; line++)
  {
   unsigned  node = 0;
    
    while( node < nodes_per_line)
    {
      unsigned connection = node+1;
      while (voltages[line][connection] - voltages[line][node] < 0.02 && connection < nodes_per_line) // If within 0.02V 
      {
        connection++;
      }

      if (node != connection && connection < nodes_per_line)
      {
        Serial.print("Line ");
        Serial.print(line);
        Serial.print(": ");
        Serial.print(node);
        Serial.print(" connected to ");
        Serial.println(connection);

        node = connection + 1;
        
      }
    }
  }

      

  counter = 0;
    for (unsigned line = 0; line < lines; line++)
    {
      Serial.print("Line ");
      Serial.print(line);
      Serial.print(":\t ");
      
      for (unsigned i = 0; i < nodes_per_line; i++)
      {
          Serial.print(voltages[line][i]);
          Serial.print("\t");
      }
      
      Serial.print ("\n");
    }
    Serial.print ("\n");



/* Testing pins */
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

bool same_voltage(float v1, float v2)
{
	return (v1 <= 1.01*v2) && (v1 >= 0.99*v2); 
}


void detect_rods(rod_view_t rods)
{
	for (unsigned i 
}

