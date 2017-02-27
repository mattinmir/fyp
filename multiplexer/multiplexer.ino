#define NODES_PER_LINE              19
#define NO_OF_LINES                 10
#define INPUT_VOLTAGE               5.0f
#define MEASUREMENT_PRECISION       0.01 

typedef float voltage_view_t[NO_OF_LINES][NODES_PER_LINE]; // Holds voltages at each node
typedef unsigned rod_view_t[NO_OF_LINES][NODES_PER_LINE]; // Holds representation of board in rod format 

voltage_view_t voltages; // Voltages at each node 
rod_view_t rods; // Representation of board in rod format 
unsigned intermediary_sel[] = { /*lsb*/ 25, 26, 27 /*msb*/ }; // Select pins for intermediary multiplexers
unsigned output_sel[] = { /*lsb*/ 22, 23, 24 /*msb*/ }; // Select pins to choose ouput of which multiplexer
unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned counter = 0;




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
  for (unsigned line = 0; line < NO_OF_LINES; line++)
  {
      // For each muliplexer
     for (unsigned i = 0; i < 3; i++)
     {
        // Set the select of the root mux to the desired intermediary output
        digitalWrite(output_sel[0], (line*3 + i) & 0x1);
        digitalWrite(output_sel[1], (line*3 + i) & 0x2);
        digitalWrite(output_sel[2], (line*3 + i) & 0x4);
                
        // For each input to the intermediary multiplexer
        for (unsigned short j = 0; j < 8 && (i*8 + j) < NODES_PER_LINE; j++) // Room for 24 reads per intermediary set but only using NODES_PER_LINE of them
        { 
          // Set select to that input
          digitalWrite(intermediary_sel[0], j & 0x1); // Bitmask bit 1
          digitalWrite(intermediary_sel[1], j & 0x2); // Bitmask bit 2
          digitalWrite(intermediary_sel[2], j & 0x4); // Bitmask bit 3

          // Take reading, store in respective array element
          voltages[line][(i*8 + j)] = analogRead(output) * (INPUT_VOLTAGE / 1023.0);
        }  
      }     
    }


    rod_view_t new_rods = detect_rods(voltages);

    if(state_has_changed(rods, new_rods))
    {
      rods = new_rods;
      print_rod_view(rods);
      /*TODO: Send message to server*/
    }


    

/* Find connections */
/*
  // For each line
  for (unsigned line = 0; line < NO_OF_LINES; line++)
  {
    unsigned node = 0;
    
    while(node < NODES_PER_LINE)
    {
      unsigned connection = node + 1; 
      while (voltages[line][connection] - voltages[line][node] < 0.02 && connection < NODES_PER_LINE) // If within 0.02V 
      {
        connection++;
      }

      if (node != connection && connection < NODES_PER_LINE)
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
    for (unsigned line = 0; line < NO_OF_LINES; line++)
    {
      Serial.print("Line ");
      Serial.print(line);
      Serial.print(":\t ");
      
      for (unsigned i = 0; i < NODES_PER_LINE; i++)
      {
          Serial.print(voltages[line][i]);
          Serial.print("\t");
      }
      
      Serial.print ("\n");
    }
    Serial.print ("\n");
*/


/* Testing pins */
//   digitalWrite(output_sel[0], 0);
//   digitalWrite(output_sel[1], 1);
//   digitalWrite(output_sel[2], 0);
//   digitalWrite(intermediary_sel[0], 0);
//   digitalWrite(intermediary_sel[1], 1);
//   digitalWrite(intermediary_sel[2], 0);
//   
//   Serial.print(analogRead(output) * (INPUT_VOLTAGE / 1023.0));
//   Serial.print ("\n");
}

// Returns true if voltages are within 2*MEASUREMENT_PRECISION of each other
bool same_voltage(float v1, float v2)
{
  unsigned upper_limit = 1 + MEASUREMENT_PRECISION;
  unsigned lower_limit = 1 - MEASUREMENT_PRECISION;

	return (v1 <= upper_limit*v2) && (v1 >= lower_limit*v2); 
}


rod_view_t detect_rods(const voltage_view_t &v)
{
  rod_view_t rods;

	// For each line
  for (unsigned line = 0; line < NO_OF_LINES; line++)
  {
    unsigned head;
    unsigned count;
    bool mid_rod = false;
    unsigned node = 0;

    // For each node on the line
    while (node < NODES_PER_LINE)
    { 
      if  ( !mid_rod && // If not currently tracing a rod
            node + 1 < NODES_PER_LINE && // and not at the end of the line
            same_voltage(v[line][node], v[line][node+1] // and the next node has the same voltage as the current one
          ) 
      {
        mid_rod = true; // Begin tracing a rod
        head = node; // Note the start of the rod 
        count = 2; // Keep count of length of rod
        node += 2; // Already know the first two nodes are part of the rod 
      }

      // If currently tracing a rod
      else if (mid_rod)
      {
        // If current node same voltage as the head
        if (same_voltage(v[line][node], v[line][head])
        {
          // Increase the trce one step
          count++;
          node++;
        }

        // If end of rod
        else
        {
          // Each rod covers 2x as many nodes as its length
          rod_length = count / 2;

          // Mark the rod in the rod_view structure
          for (unsigned i = head; i < node; i++)
          {
            rods[line][i] = rod_length;
          }

          // End of rod
          mid_rod = false;
        } 
      }
    }
  }
}

bool state_has_changed(rod_view_t rods_old, rod_view_t rods_new)
{
  // For every cell in the rod_view
  for (unsigned i = 0; i < NO_OF_LINES; i++)
  {
    for (unsigned j = 0; j < NODES_PER_LINE; j++)
    {
      // If any cell is different, return true
      if (rods_old[i][j] != rods_new[i][j])
      {
        return true;
      }
    }
  }

  // If nothing has changed, return false
  return false;
}

void print_rod_view(rod_view_t r)
{
  for (unsigned i = 0; i < NO_OF_LINES; i++)
  {
    for (unsigned j = 0; j < NODES_PER_LINE; j++)
    {
      Serial.print (r[i][j]);
      Serial.print ("\t");
    }
    Serial.print ("\n");
  }
}