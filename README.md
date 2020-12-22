
[![Arduino CI](https://github.com/RobTillaart/I2C_EEPROM/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/I2C_EEPROM/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/I2C_EEPROM.svg?maxAge=3600)](https://github.com/RobTillaart/I2C_EEPROM/releases)

# I2C_EEPROM

Arduino Library for external I2C EEPROM - 24LC256, 24LC64

## Description

Library to access external I2C EEPROM. 

The interface is pretty straightforward

- **begin()** constructor
- **begin(sda, scl)** constructor for ESP32
- **writeByte(address, value)** write a single byte
- **writeBlock(address, buffer, length)** 
- **setBlock(address, value, length)** e.g. use to clear I2C EEPROM
- **readByte(address)** - read a single byte from a given address
- **readBlock(address, buffer, length)**
- **determineSize()**


## Limitation

The library does not offer multiple EEPROMS as one 
continuous storage device. 


## Operational

See examples

