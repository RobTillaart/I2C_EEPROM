//
//    FILE: I2C_eeprom_test_performance.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: show/test I2C_EEPROM library
//


#include "Wire.h"
#include "I2C_eeprom.h"


//for decimal display uncomment below two lines
#define DISPLAY_DECIMAL
#define BLOCK_TO_LENGTH 10

//for hex display uncomment below two lines
//#define DISPLAY_HEX
//#define BLOCK_TO_LENGTH 16


#define MEMORY_SIZE 0x2000 // total bytes can be accessed 24LC64 = 0x2000 (maximum address = 0x1FFF)


I2C_eeprom ee(0x50, MEMORY_SIZE);

uint32_t start, diff, totals = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  while (!Serial); // wait for Serial port to connect. Needed for Leonardo only

  ee.begin();

  for (uint32_t speed = 100000; speed <= 1000000; speed += 100000)
  {
    Wire.setClock(speed);
    Serial.print("SPEED: ");
    Serial.println(speed);
    delay(10);
    test();
  }

  Serial.println("\ndone...");
}

void loop()
{
}

////////////////////////////////////////////////////////////////////

void test()
{
  char data2[] = "0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999A"; 
    
  totals = 0;
  Serial.print("\nTEST: timing writeByte()\t");
  uint32_t start = micros();
  ee.writeByte(10, 1);
  uint32_t diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  Serial.print("TEST: timing writeBlock(50)\t");
  start = micros();
  ee.writeBlock(10, (uint8_t *) &data2, 50);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  Serial.print("TEST: timing readByte()\t\t");
  start = micros();
  ee.readByte(10);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  Serial.print("TEST: timing readBlock(50)\t");
  start = micros();
  ee.readBlock(10, (uint8_t *) &data2, 50);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  Serial.print("TOTALS: ");
  Serial.println(totals);
  totals = 0;

  // same tests but now with a 5 millisec delay in between.
  delay(5);

  Serial.print("\nTEST: timing writeByte()\t");
  start = micros();
  ee.writeByte(10, 1);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  delay(5);

  Serial.print("TEST: timing writeBlock(50)\t");
  start = micros();
  ee.writeBlock(10, (uint8_t *) &data2, 50);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  delay(5);

  Serial.print("TEST: timing readByte()\t\t");
  start = micros();
  ee.readByte(10);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  delay(5);

  Serial.print("TEST: timing readBlock(50)\t");
  start = micros();
  int xx = ee.readBlock(10, (uint8_t *) &data2, 50);
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.println(diff);
  totals += diff;

  Serial.print("TOTALS: ");
  Serial.println(totals);
  totals = 0;

  // does it go well?
  Serial.println(xx);
}


void dumpEEPROM(uint16_t memoryAddress, uint16_t length)
{
#ifdef DISPLAY_DECIMAL
  Serial.print("\t  ");
#endif
#ifdef DISPLAY_HEX
  Serial.print("\t ");
#endif
  for (int x = 0; x < BLOCK_TO_LENGTH; x++) {
    if (x != 0) {
#ifdef DISPLAY_DECIMAL
      Serial.print("    ");
#endif
#ifdef DISPLAY_HEX
      Serial.print("   ");
#endif
    }
#ifdef DISPLAY_DECIMAL
    Serial.print(x);
#endif
#ifdef DISPLAY_HEX
    Serial.print(x, HEX);
#endif
  }
  Serial.println();

  // block to defined length
  memoryAddress = memoryAddress / BLOCK_TO_LENGTH * BLOCK_TO_LENGTH;
  length = (length + BLOCK_TO_LENGTH - 1) / BLOCK_TO_LENGTH * BLOCK_TO_LENGTH;

  byte b = ee.readByte(memoryAddress);
  for (unsigned int i = 0; i < length; i++)
  {
    char buf[6];
    if (memoryAddress % BLOCK_TO_LENGTH == 0)
    {
      if (i != 0) {
        Serial.println();
      }
#ifdef DISPLAY_DECIMAL
      sprintf(buf, "%05d", memoryAddress);
#endif
#ifdef DISPLAY_HEX
      sprintf(buf, "%04X", memoryAddress);
#endif
      Serial.print(buf);
      Serial.print(":\t");
    }
#ifdef DISPLAY_DECIMAL
    sprintf(buf, "%03d", b);
#endif
#ifdef DISPLAY_HEX
    sprintf(buf, "%02X", b);
#endif
    Serial.print(buf);
    b = ee.readByte(++memoryAddress);
    Serial.print("  ");
  }
  Serial.println();
}
// END OF FILE
