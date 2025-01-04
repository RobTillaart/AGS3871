
[![Arduino CI](https://github.com/RobTillaart/AGS3871/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/AGS3871/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/AGS3871/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/AGS3871/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/AGS3871/actions/workflows/jsoncheck.yml)
[![GitHub issues](https://img.shields.io/github/issues/RobTillaart/AGS3871.svg)](https://github.com/RobTillaart/AGS3871/issues)

[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/AGS3871/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/AGS3871.svg?maxAge=3600)](https://github.com/RobTillaart/AGS3871/releases)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/robtillaart/library/AGS3871.svg)](https://registry.platformio.org/libraries/robtillaart/AGS3871)


# AGS3871

Arduino library for the AGS3871 - CarbonMonoxide CO sensor.


#### Description

**Experimental**

This library is still experimental, so please use with care.



Note this library is not meant to replace professional monitoring systems.



### Related

- https://github.com/RobTillaart/AGS02MA TVOC sensor
- https://github.com/RobTillaart/AGS3871
- https://www.renesas.com/us/en/document/whp/overview-tvoc-and-indoor-air-quality


## I2C

### PIN layout from left to right

|  Front L->R  |  Description  |
|:------------:|:--------------|
|   pin 1      |   VDD +       |
|   pin 2      |   SDA data    |
|   pin 3      |   GND         |
|   pin 4      |   SCL clock   |


#### WARNING - LOW SPEED

The sensor uses I2C at very low speed <= 30 KHz.
For an Arduino UNO the lowest speed supported is about 30.4KHz (TWBR = 255) which works.
First runs with Arduino UNO indicate 2 failed reads in > 500 Reads, so less than 1%

Tests with ESP32 / ESP8266 at 30 KHz look good,
tests with ESP32 at lower clock speeds are to be done but expected to work.

The library sets the clock speed to 30 KHz (for non AVR) during operation
and resets it default to 100 KHz after operation.
This is done to minimize interference with the communication of other devices.
The reset clock speed can be changed with **setI2CResetSpeed(speed)** e.g. to 200 or 400 KHz.


#### I2C multiplexing

Sometimes you need to control more devices than possible with the default
address range the device provides.
This is possible with an I2C multiplexer e.g. TCA9548 which creates up 
to eight channels (think of it as I2C subnets) which can use the complete 
address range of the device. 

Drawback of using a multiplexer is that it takes more administration in 
your code e.g. which device is on which channel. 
This will slow down the access, which must be taken into account when
deciding which devices are on which channel.
Also note that switching between channels will slow down other devices 
too if they are behind the multiplexer.

- https://github.com/RobTillaart/TCA9548



### Please report your experiences.

If you have a AGS3871 device, please let me know your experiences
with the sensor and this (or other) library.


## Interface

```cpp
#include "AGS3871.h"
```

### Constructor

- **AGS3871(uint8_t deviceAddress = 26, TwoWire \*wire = &Wire)** constructor, 
with default address and default I2C interface.
- **bool begin()** initialize the library.
- **bool isConnected()** returns true if device address can be seen on I2C.
- **void reset()** reset internal variables.


### Timing

- **bool isHeated()** returns true if 2 minutes have passed after startup (call of **begin()** ).
Otherwise the device is not optimal ready.
According to the datasheet the preheating will improve the quality of the measurements.
- **uint32_t lastRead()** last time the device is read, timestamp is in milliseconds since start.
Returns 0 if **readPPB()** or **readUGM3()** is not called yet.
This function allows to implement sort of asynchronous wait.
One must keep reads at least 1.5 seconds but preferred 3 seconds apart according to the datasheet.


### Administration

- **bool setAddress(const uint8_t deviceAddress)** sets a new address for the sensor.
If function succeeds the address changes immediately and will be persistent over a reboot.
- **uint8_t getAddress()** returns the set address. Default the function will return 26 or 0x1A.
- **uint8_t getSensorVersion()** reads sensor version from device.
If the version cannot be read the function will return 255.
(My test sensors all return version 117, version 118 is reported)
- **uint32_t getSensorDate()** (experimental) reads bytes from the sensor that seem to indicate the production date(?). This date is encoded in an uint32_t to minimize footprint as it is a debug function.

```cpp
uint32_t dd = sensor.getSensorDate();
Serial.println(dd, HEX);   //  prints YYYYMMDD e.g. 20210203
```


#### I2C clock speed

The library sets the clock speed to 25 KHz during operation
and resets it to 100 KHz after operation.
This is done to minimize interference with the communication of other devices.
The following function can change the I2C reset speed to e.g. 200 or 400 KHz.

- **void setI2CResetSpeed(uint32_t speed)** sets the I2C speed the library need to reset the I2C speed to.
- **uint32_t getI2CResetSpeed()** returns the value set. Default is 100 KHz.


#### setMode

The default mode at startup of the sensor is PPB = parts per billion.

- **bool setPPBMode()** sets device in PartPerBillion mode. Returns true on success.
- **bool setUGM3Mode()** sets device in micro gram per cubic meter mode. Returns true on success.
- **uint8_t getMode()** returns mode set. 0 = PPB, 1 = UGm3, 255 = not set.



#### Read the sensor

WARNING: The datasheet advises to take 3 seconds between reads.
Tests gave stable results at 1.5 second intervals.
Use this faster rate at your own risk.

- **uint32_t readPPB()** reads PPB (parts per billion) from device.
Typical value should be between 1 .. 999999.
Returns **lastPPB()** value if failed so one does not get sudden jumps in graphs.
Check **lastStatus()** and **lastError()** to get more info about success.
Time needed is ~35 milliseconds.
- **uint32_t readUGM3()** reads UGM3 (microgram per cubic meter) current value from device.
Typical values depend on the molecular weight of the TVOC.
Returns **lastUGM3()** if failed so one does not get sudden jumps in graphs.
- **float readPPM()** returns parts per million (PPM).
This function is a wrapper around readPPB().
Typical value should be between 0.01 .. 999.99
- **float readMGM3()** returns milligram per cubic meter.
- **float readUGF3()** returns microgram per cubic feet.


#### Error Codes

|  ERROR_CODES                |  value  |
|:----------------------------|:-------:|
|  AGS3871_OK                 |     0   |
|  AGS3871_ERROR              |   -10   |
|  AGS3871_ERROR_CRC          |   -11   |
|  AGS3871_ERROR_READ         |   -12   |
|  AGS3871_ERROR_NOT_READY    |   -13   |


#### Cached values

- **float lastPPM()** returns last readPPM (parts per million) value (cached).
- **uint32_t lastPPB()** returns last read PPB (parts per billion) value (cached). Should be between 1..999999.
- **uint32_t lastUGM3()** returns last read UGM3 (microgram per cubic meter) value (cached).


#### Calibration

- **bool zeroCalibration()** to be called after at least 5 minutes in fresh air.
See example sketch.
- **bool manualZeroCalibration(uint16_t value = 0)** Set the zero calibration value manually.
To be called after at least 5 minutes in fresh air.
  - For v117: 0-65535 = automatic calibration.
  - For v118: 0 = automatic calibration, 1-65535 manual calibration.
- **bool getZeroCalibrationData(ZeroCalibrationData &data)** fills a data struct with the 
current zero calibration status and value. 
Returns true on success.


#### Other

- **bool readRegister(uint8_t address, RegisterData &reg)** fills a data struct with the chip's register data at that address.
Primarily intended for troubleshooting and analysis of the sensor. Not recommended to build applications on top of this method's raw data.
Returns true when the **RegisterData** is filled, false when the data could not be read.
Note: unlike other public methods, CRC errors don't return false or show up in `lastError()`, 
instead the CRC result is stored in `RegisterData.crcValid`.
- **int lastError()** returns last error.
- **uint8_t lastStatus()** returns status byte from last read.
Read datasheet or table below for details. A new read is needed to update this.
- **uint8_t dataReady()** returns RDY bit from last read.


#### Status bits.

|  bit  |  description                        |  notes  |
|:-----:|:------------------------------------|:--------|
|  7-4  |  internal use                       |
|  3-1  |  000 = PPB  001 = uG/M3             |
|   0   |  RDY bit  0 = ready  1 = not ready  |  1 == busy


## Future

#### Must

- improve documentation
  - references?
- test with hardware

#### Should

- check the mode bits of the status byte with internal \_mode.
  - maximize robustness of state
- test with hardware
- test with different processors

#### Could

- elaborate error handling.
- create an async interface for **readPPB()** if possible
  - delay(30) blocks performance ==> async version of **readRegister()**
  - could introduce complex I2C speed handling...
  - separate state - request pending or so?
- move code to .cpp?

#### Wont


## Support

If you appreciate my libraries, you can support the development and maintenance.
Improve the quality of the libraries by providing issues and Pull Requests, or
donate through PayPal or GitHub sponsors.

Thank you,

