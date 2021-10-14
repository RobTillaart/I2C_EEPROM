#include "I2C_eeprom.h"

char cihaz_adi[50]={"TwinsLED"};

I2C_eeprom ee(0x51, I2C_DEVICESIZE_24LC512);

Setup()
{
Wire.begin();
Wire.setClock(400000);
ee.begin();

strcpy(cihaz_adi, request->arg(i).c_str());
pass_len1 = strlen(cihaz_adi);
cihaz_adi[5]='\0';
ee.writeBlock(600, (uint8_t *)&cihaz_adi, 4);
ee.writeByte(350, pass_len1);

pass_len1 = ee.readByte(350);
ee.readBlock(600, (uint8_t *)cihaz_adi, pass_len1);

}
