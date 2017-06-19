#include <SoftwareSerial.h>


#define NODES_PER_LINE              20
#define GRID_SQUARES_PER_LINE       ((NODES_PER_LINE)/(2))
#define NO_OF_LINES                 1
#define INPUT_VOLTAGE               5.0f
#define MEASUREMENT_PRECISION       0.03f




typedef float voltage_view_t[NO_OF_LINES][NODES_PER_LINE]; // Holds voltages at each node
typedef unsigned rod_view_t[NO_OF_LINES][GRID_SQUARES_PER_LINE]; // Holds representation of board in rod format

SoftwareSerial bt(10, 11); // RX, TX

voltage_view_t voltages; // Voltages at each node
rod_view_t rods; // Representation of board in rod format


// Higher level = closer to resistors
// Lower level = closer to Arduino
unsigned level2_sel[] = { /*lsb*/ 25, 26, 27 /*msb*/ };
unsigned level1_sel[] = { /*lsb*/ 22, 23, 24 /*msb*/ };

unsigned debug_pin = 53;
bool debug = false;

unsigned output = 0;   // Analogue pin to read voltage reading from
unsigned counter = 0;

void detect_rods(const voltage_view_t &v, rod_view_t &rods);
void send_msg(String code, String msg, SoftwareSerial& conn);
String rods_to_json(rod_view_t rods);


void setup(){
  Serial.begin(9600);  
  bt.begin(9600);
  pinMode(debug_pin, INPUT);
  
  for(unsigned pin = 0; pin < 3; pin++) // setup select pins
  {
    pinMode(level2_sel[pin], OUTPUT);
    pinMode(level1_sel[pin], OUTPUT);
  }

  for (unsigned i = 0; i < NO_OF_LINES; i++)
  {
    for (unsigned j = 0; j < GRID_SQUARES_PER_LINE; j++)
    {
      rods[i][j] = 0;
    }
  }

  Serial.println ("Begin\n");
}


void loop()
{
  debug = digitalRead(debug_pin);

  // For each line
  for (unsigned line = 0; line < NO_OF_LINES; line++)
  {
      // For each muliplexer
     for (unsigned i = 0; i < 3; i++)
     {
        // Set the select of the root mux to the desired level2 output
        digitalWrite(level1_sel[0], (line*3 + i) & 0x1);
        digitalWrite(level1_sel[1], (line*3 + i) & 0x2);
        digitalWrite(level1_sel[2], (line*3 + i) & 0x4);

        // For each input to the level2 multiplexer
        for (unsigned short j = 0; j < 8 && (i*8 + j) < NODES_PER_LINE; j++) // Room for 24 reads per level2 set but only using NODES_PER_LINE of them
        {
          // Set select to that input
          digitalWrite(level2_sel[0], j & 0x1); // Bitmask bit 1
          digitalWrite(level2_sel[1], j & 0x2); // Bitmask bit 2
          digitalWrite(level2_sel[2], j & 0x4); // Bitmask bit 3

          // Take reading, store in respective array element
          voltages[line][(i*8 + j)] = analogRead(output) * (INPUT_VOLTAGE / 1023.0);
        }
      }
    }


    rod_view_t new_rods;
    detect_rods(voltages, new_rods);

    delay(100);
    if(state_has_changed(rods, new_rods))
    {
      for(unsigned i = 0; i < NO_OF_LINES; i++)
      {
        memcpy(&rods[i], &new_rods[i], sizeof(new_rods[0]));
      }
      Serial.println ("\n");

      print_rod_view(rods);


      send_msg("0", rods_to_json(rods), bt);
      while (bt.available())
      {
        Serial.write(bt.read());
      }

   }

}

// Returns true if voltages are within 2*MEASUREMENT_PRECISION of each other
bool same_voltage(float v1, float v2)
{
  //float upper_limit = 1.0 + MEASUREMENT_PRECISION;
  //float lower_limit = 1.0 - MEASUREMENT_PRECISION;
  float leeway = INPUT_VOLTAGE * MEASUREMENT_PRECISION;

  return (v1 <= v2+leeway) && (v1 >= v2-leeway);
}


void detect_rods(const voltage_view_t &v, rod_view_t &rods)
{


  // For each line
  for (unsigned line = 0; line < NO_OF_LINES; line++)
  {
    unsigned head;
    unsigned count;
    bool mid_rod = false;
    unsigned node = 0;

    if(debug)
    {
      delay(1000);
      Serial.print ("\n");
      Serial.print (line);
      Serial.print ("\n");

      Serial.print ("node");
      Serial.print ('\t');
      Serial.print ("mid_rod");
      Serial.print ('\t');
      Serial.print ("!end");
      Serial.print ('\t');
      Serial.print ("=Next");
      Serial.print ('\t');
      Serial.print ("=Head");
      Serial.print ('\t');
      Serial.print ("head");
      Serial.print ('\t');
      Serial.print ("count");
      Serial.print ('\t');
      Serial.print ("v node");
      Serial.print ('\t');
      Serial.print ("v node+1");
      Serial.print ("\n");
    }

    // For each node on the line
    while (node < NODES_PER_LINE)
    {
      if(debug)
      {
        Serial.print (node);
        Serial.print ('\t');
        Serial.print (mid_rod);
        Serial.print ('\t');
        Serial.print (node + 1 < NODES_PER_LINE);
        Serial.print ('\t');
        Serial.print (same_voltage(v[line][node], v[line][node+1]));
        Serial.print ('\t');
        Serial.print (same_voltage(v[line][node], v[line][head]));
        Serial.print ('\t');
        Serial.print (head);
        Serial.print ('\t');
        Serial.print (count);
        Serial.print ('\t');
        Serial.print (v[line][node]);
        Serial.print ('\t');
        Serial.print (v[line][node+1]);
        Serial.print ("\n");
      }

      if  ( !mid_rod && // If not currently tracing a rod
            node + 1 < NODES_PER_LINE // and not at the end of the line
          )
      {
        // If the next node has the same voltage as the current one
        if(same_voltage(v[line][node], v[line][node+1]))
        {
          mid_rod = true; // Begin tracing a rod
          head = node; // Note the start of the rod
          count = 2; // Keep count of length of rod
          node += 2; // Already know the first two nodes are part of the rod

           // If end of line then end of rod
          if (node + 1 > NODES_PER_LINE)
          {
            // Each rod covers 2x as many nodes as its length
            unsigned rod_length = (count+1) / 2;
            unsigned rod_begin = head/2;

            // Mark the rod in the rod_view structure
            for (unsigned i = 0; i < rod_length; i++)
            {
              rods[line][rod_begin+i] = rod_length;
            }
          }
        }

        // If there is no rod present
        else
        {
          rods[line][node/2] = 0; // Divide by 2 because there are 2 nodes for every grid square
          node+=2; // If we are not in a rod and there wasnt a connection in the first spot of the cell, there won't be in the second
        }
      }

      // If currently tracing a rod
      else if (mid_rod)
      {
        bool still_mid_rod =  same_voltage(v[line][node], v[line][head]);

        // If current node same voltage as the head
        if (still_mid_rod)
        {
          // Increase the trace one step
          count++;
          node++;

          // If end of line then end of rod
          if (node + 1 == NODES_PER_LINE)
          {
            // Each rod covers 2x as many nodes as its length
            unsigned rod_length = (count+1) / 2;
            unsigned rod_begin = head/2;

            // Mark the rod in the rod_view structure
            for (unsigned i = 0; i < rod_length; i++)
            {
              rods[line][rod_begin+i] = rod_length;

            }
          }
        }

        // If end of rod
        else if(!still_mid_rod)
        {
          // Each rod covers 2x as many nodes as its length
          unsigned rod_length = (count+1) / 2;
          unsigned rod_begin = head/2;

          // Mark the rod in the rod_view structure
          for (unsigned i = 0; i < rod_length; i++)
          {
            rods[line][rod_begin+i] = rod_length;

          }

          // End of rod
          mid_rod = false;
          count = 0;

        }
      }

      else if (  !mid_rod && // If not currently tracing a rod
                 node + 1 == NODES_PER_LINE // and at the end of the line
              )
      {
        break;
      }


    }
  }
}

bool state_has_changed(rod_view_t rods_old, rod_view_t rods_new)
{
  // For every cell in the rod_view
  for (unsigned i = 0; i < NO_OF_LINES; i++)
  {
    for (unsigned j = 0; j < GRID_SQUARES_PER_LINE; j++)
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
    for (unsigned j = 0; j < GRID_SQUARES_PER_LINE; j++)
    {
      Serial.print (r[i][j]);
      Serial.print ("\t");
    }
    Serial.print ("\n");
  }
  Serial.print ("\n");
}

void send_msg(String code, String msg, SoftwareSerial &conn)
{
  msg = code + msg;
  int msg_len = msg.length();
  String msg_len_str = String(msg_len);
  
  String padded_msg_len_str = msg_len_str;
  while (padded_msg_len_str.length() < 4)
  {
    padded_msg_len_str = "0" + padded_msg_len_str;
  }

  String sized_msg =  padded_msg_len_str + msg;


  conn.write(sized_msg.c_str());   
}

String rods_to_json(rod_view_t rods)
{
  String json = "";
  json += '[';
  for (unsigned i = 0; i < NO_OF_LINES; i++)
  {
    json += '[';
    for (unsigned j = 0; j < GRID_SQUARES_PER_LINE; j++)
    {
      json += rods[i][j];
      if(j < GRID_SQUARES_PER_LINE - 1)
      {
        json += ',';
      }
    }
    json += ']';
     if(i < NO_OF_LINES - 1)
     {
      json += ',';
     }
  }
  json += ']';

  return json;
  
 }

