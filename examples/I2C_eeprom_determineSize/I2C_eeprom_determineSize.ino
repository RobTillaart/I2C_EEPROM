//
//    FILE: I2C_eeprom_determineSize.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: test determinsize function
//


#include "Wire.h"
#include "I2C_eeprom.h"


#define MEMORY_SIZE 0x2000 //total bytes can be accessed 24LC64 = 0x2000 (maximum address = 0x1FFF)

I2C_eeprom ee(0x50, MEMORY_SIZE);

uint32_t start, diff;


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  ee.begin();
  if (! ee.isConnected())
  {
    Serial.println("ERROR: Can't find eeprom\nstopped...");
    while (1);
  }

  Serial.print("I2C_EEPROM_VERSION: ");
  Serial.println(I2C_EEPROM_VERSION);

  Serial.println("\nDetermine size");
  delay(10);

  start = micros();
  int size = ee.determineSize(true);  // debug param
  diff = micros() - start;
  Serial.print("TIME: ");
  Serial.print(diff);
  Serial.println(" us.");
  if (size > 0)
  {
    Serial.print("SIZE: ");
    Serial.print(size);
    Serial.println(" KB.");
  }
  if (size == 0)
  {
    Serial.println("WARNING: Can't determine eeprom size");
  }
  Serial.println("Done...");
}


void loop()
{
}

// -- END OF FILE --
