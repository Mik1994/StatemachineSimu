#include <Wire.h>
#define ADDRESS 0x70

#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_CMD_BRIGHTNESS 0xE0

//glider
byte grid[] = {B01000000,
               B00100000,
               B11100000,
               B00000000,
               B00000000,
               B00000000,
               B00000000,
               B00000000
              };
void setup() {

  // TODO: set up your other stuff...
  //Wire zeugs
  Wire.begin();
  // I2C oscillator on
  Wire.beginTransmission(ADDRESS);
  Wire.write(0x21);
  Wire.endTransmission();
  // Set Brightness (max brightness == 15)
  Wire.beginTransmission(ADDRESS);
  Wire.write(HT16K33_CMD_BRIGHTNESS | 14);
  Wire.endTransmission();
  // No blinking
  Wire.beginTransmission(ADDRESS);
  Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON);
  Wire.endTransmission();
}


void loop() {

  // WIRE zeugs
  updateDisplay();
  //Compute next based on cur grid
  compute();
}

/**********************************************/
/*            Game of Live                    */
/**********************************************/
int countNeighbors(byte curGrid[], int x, int y) {
  int sum = 0;
  for (byte i = -1; i < 2; i++) {
    for (byte j = -1; j < 2; j++) {
      int col = (x + i + 8) % 8;
      int row = (y + j + 8) % 8;
      sum += bitRead(curGrid[col], row);
    }
  }
  return sum -= bitRead(curGrid[x], y); //ignore my self
}

void compute() {
  byte next[] = {B00000000,
                 B00000000,
                 B00000000,
                 B00000000,
                 B00000000,
                 B00000000,
                 B00000000,
                 B00000000
                };
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {

      int neighbors = countNeighbors(grid, i, j);
      // Rules
      if ( bitRead(grid[i], j) == 0 && neighbors == 3) {
        // Any dead cell with exactly htree live neighbours becomes a live Cell
        bitSet(next[i], j);
      } else if (bitRead(grid[i], j) == 1 && (neighbors < 2 || neighbors > 3)) {
        bitClear(next[i], j);
      } else {
        bitWrite(next[i], j, bitRead(grid[i], j));
      }

    }
  }
  *grid = next;
}

/**********************************************/
/*             Update                         */
/**********************************************/
void updateDisplay() {

  Wire.beginTransmission(ADDRESS);
  Wire.write(0);  // Select base register
  // FOR EACH ROW
  for (byte i = 0; i < 8; i++) {
    // Achtung erste Spalte an der letzten Position im Zeilen-Bitvektor
    //  Bit: 12345678 =>  23456781
    byte ic2 = ((grid[i] & 0x80) ? 0x01 : 0x00) | (grid[i] << 1);
    Wire.write(ic2);
    Wire.write(0);
  }
  Wire.endTransmission();

}

