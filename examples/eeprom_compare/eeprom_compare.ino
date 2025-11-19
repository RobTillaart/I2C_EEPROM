//
//    FILE: eeprom_compare.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo I2C_EEPROM library
//     URL: https://github.com/RobTillaart/I2C_EEPROM
//
//  uses 2x 24LC256 (32KB) EEPROM
//
//  second eeprom must be at least equal in size.

#include "Wire.h"
#include "I2C_eeprom.h"

//  I2C buffer size AVR = 32
//  I2C buffer size ESP32 = 256
int BUFFER_SIZE = 16;

//  adjust addresses if needed
int SOURCE = 0x50;
int DESTINATION = 0x51;

//  adjust type if needed
I2C_eeprom ee1(SOURCE, I2C_DEVICESIZE_24LC256);
I2C_eeprom ee2(DESTINATION, I2C_DEVICESIZE_24LC256);

uint32_t start, stop;


void setup()
{
  Serial.begin(115200);
  while (!Serial);  //  wait for Serial port to connect. Needed for Leonardo only
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("I2C_EEPROM_VERSION: ");
  Serial.println(I2C_EEPROM_VERSION);
  Serial.println();

  Wire.begin();

  ee1.begin();
  if (! ee1.isConnected())
  {
    Serial.println("ERROR: Can't find eeprom 1\nstopped...");
    while (1);
  }

  ee2.begin();
  if (! ee2.isConnected())
  {
    Serial.println("ERROR: Can't find eeprom 2\nstopped...");
    while (1);
  }

  start = millis();
  for (uint32_t addr = 0; addr < ee1.getDeviceSize(); addr += BUFFER_SIZE)
  {
    uint8_t buffer1[BUFFER_SIZE];
    uint8_t buffer2[BUFFER_SIZE];
    bool b1 = ee1.readBlock(addr, buffer1, BUFFER_SIZE);
    if (b1 == false)
    {
      Serial.print("Read error (1) at block: ");
      Serial.println(addr, HEX);
    }
    bool b2 = ee2.readBlock(addr, buffer2, BUFFER_SIZE);
    if (b2 == false)
    {
      Serial.print("Read error (2) at block: ");
      Serial.println(addr, HEX);
    }
    if (memcmp(buffer1, buffer2, BUFFER_SIZE) != 0)
    {
      Serial.print("Compare error at block: ");
      Serial.println(addr, HEX);
      //  optional dump blocks.
      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        Serial.print(buffer1[i], HEX);
        Serial.print('\t');
      }
      Serial.println();
      for (int i = 0; i < BUFFER_SIZE; i++)
      {
        Serial.print(buffer2[i], HEX);
        Serial.print('\t');
      }
      Serial.println();
    }
  }
  stop = millis();
  Serial.print("TIME: ");
  Serial.println(stop - start);
  Serial.println("\ndone...");
}


void loop()
{
}


//  -- END OF FILE --
