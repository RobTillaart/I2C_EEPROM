//
//    FILE: I2C_eeprom_update.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo I2C_EEPROM library - updateByte
//

#include <Wire.h>
#include <I2C_eeprom.h>

#define MEMORY_SIZE 0x2000 // total bytes can be accessed 24LC64 = 0x2000 (maximum address = 0x1FFF)

I2C_eeprom ee(0x50, MEMORY_SIZE);

uint32_t start, dur1, dur2;

void setup() 
{
  ee.begin();
  Serial.begin(115200); 
  while (!Serial); // wait for SERIAL_OUT port to connect. Needed for Leonardo only

  Serial.println(__FILE__);
  Serial.print("VERSION: ");
  Serial.println(I2C_EEPROM_VERSION);

  Serial.println("\nTEST: determine size");
  int size = ee.determineSize();
  if (size > 0)
  {
    Serial.print("SIZE: ");
    Serial.print(size);
    Serial.println(" KB");
  } 
  else if (size == 0)
  {
    Serial.println("WARNING: Can't determine eeprom size");
  }
  else
  {
    Serial.println("ERROR: Can't find eeprom\nstopped...");
    while(1);
  }

  Serial.println("\nTEST: 100x writebyte()");
  delay(10);
  ee.setBlock(0, 0, 100);  // clear first 100 bytes
  start = micros();
  for (int i = 0; i < 100; i++)
  {
    ee.writeByte(i, 0);
  }
  dur1 = micros() - start;
  Serial.print("DUR1: ");
  Serial.println(dur1);
  delay(10);
  
  Serial.println("\nTEST: 100x updateByte()");
  ee.setBlock(0, 0, 100);  // clear first 100 bytes
  start = micros();
  for (int i = 0; i < 100; i++)
  {
    ee.updateByte(i, 0);
  }
  dur2 = micros() - start;
  Serial.print("DUR2: ");
  Serial.println(dur2);
  delay(10);
  
  Serial.println("done...");
}

void loop() 
{
}

// -- END OF FILE
