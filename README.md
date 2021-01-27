
[![Arduino CI](https://github.com/RobTillaart/I2C_EEPROM/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/I2C_EEPROM/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/I2C_EEPROM.svg?maxAge=3600)](https://github.com/RobTillaart/I2C_EEPROM/releases)

# I2C_EEPROM

Arduino Library for external I2C EEPROM - 24LC256, 24LC64

## Description

This library is to access external I2C EEPROM up to 64KB (= 512 Kbit) in size.

The **I2C_eeprom_cyclic_store** interface is documented [here](README_cyclic_store.md)


## Interface

### Constructor

- **I2C_eeprom(deviceAddress)** constructor.
- **bool begin()** initializes the I2C bus and checks if the device is available on the I2C bus.
- **bool begin(sda, scl)** idem for ESP32 / ESP8266 and alike.
- **bool isConnected()** test to see if device is on the bus.


### Core functions

- **int writeByte(memoryAddress, value)** write a single byte to the specified memory address.
- **int writeBlock(memoryAddress, buffer, length)** write a buffer starting at the specified memory address. 
- **int setBlock(memoryAddress, value, length)** writes the same byte to length places starting at the specified memory address. Returns 0 if OK.
- **uint8_t readByte(memoryAddress)** read a single byte from a given address
- **uint16_t readBlock(memoryAddress, buffer, length)** read length bytes into buffer starting at specified memory address. Returns the number of bytes read, which should be length.
- **int updateByte(memoryAddress, value)** write a single byte, but only if changed. Returns 0 if value was same or write succeeded.


### Other

- **uint32_t getDeviceSize()** idem
- **uint8_t  getPageSize()** idem
- **uint32_t getLastWrite()** idem
- **uint32_t determineSize(bool debug = false)**  
function that determins the size of the EEPROM by detecting when a memory address is folded upon memory address 0. 
It is based upon the observation that memory wraps around. 
The debug flag gives some output to Serial.

**Warning**: this function has changed (again) in 1.4.0 

Testresults 

| returns |  Size        | Type  | Notes |
|:--------|:-------------|:------|:------|
|    0    | connect error |  -   | check device address / wiring |
|  65536  |  64 KB     | 24LC512 |       |
|  32768  |  32 KB     | 24LC256 |       |
|  16384  |  16 KB     | 24LC128 |       |
|   8192  |   8 KB     | 24LC64  |       |
|   4096  |   4 KB     | 24LC32  | not tested with hardware |
|   2048  |   2 KB     | 24LC16  |       |
|   1024  |   1 KB     | 24LC08  |       |
|    512  |  512 bytes | 24LC04  |       |
|    256  |  256 bytes | 24LC02  |       |
|    128  |  128 bytes | 24LC01  |       |

The function cannot detect smaller than 128 bit EEPROMS (yet).


## Limitation

The library does not offer multiple EEPROMS as one continuous storage device.



## Future

- look into support of > 64KB EEPROM
- improve error handling, write functions should return bytes written or so.
- what can we do with the print interface? (investigate)
- Wire0 .. WireN support
- investigate multi-EEPROM storage, 
- investigate format / clear all function.

## Operational

See examples

