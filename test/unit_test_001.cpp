//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2021-08-12
// PURPOSE: unit tests for the AGS02NA TVOC sensor
//          https://github.com/RobTillaart/AGS3871
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)

#include <ArduinoUnitTests.h>


#include "Arduino.h"
#include "AGS3871.h"

// writing to a virtual device does not work
// as millis() function is not implemented in 
// the Arduino-CI environment


unittest_setup()
{
  fprintf(stderr, "AGS3871_LIB_VERSION: %s\n", (char *) AGS3871_LIB_VERSION);
}


unittest_teardown()
{
}


unittest(test_constants)
{
  assertEqual(  0, AGS3871_OK);
  assertEqual(-10, AGS3871_ERROR);
  assertEqual(-11, AGS3871_ERROR_CRC);
  assertEqual(-12, AGS3871_ERROR_READ);
  assertEqual(-13, AGS3871_ERROR_NOT_READY);
  assertEqual(-14, AGS3871_ERROR_REQUEST);
}


unittest(test_base)
{
  AGS3871 AGS(26);
  Wire.begin();

  assertTrue(AGS.begin());
  assertTrue(AGS.isConnected());   // TODO - GODMODE

  assertFalse(AGS.isHeated());
  assertEqual(0, AGS.lastRead());

  assertEqual(26, AGS.getAddress());
  //assertTrue(AGS.setAddress(42));
  //assertEqual(42, AGS.getAddress());

  assertEqual(100000, AGS.getI2CResetSpeed());
  AGS.setI2CResetSpeed(400000);
  assertEqual(400000, AGS.getI2CResetSpeed());

  assertEqual(0, AGS.lastError());
  assertEqual(0, AGS.lastStatus());
}


unittest(test_mode)
{
  AGS3871 AGS(26);
  Wire.begin();

  assertTrue(AGS.begin());
  assertTrue(AGS.isConnected());   // TODO - GODMODE

  assertEqual(255, AGS.getMode());

  // assertTrue(AGS.setPPBMode());
  // assertEqual(0, AGS.getMode());

  // assertTrue(AGS.setUGM3Mode());
  // assertEqual(1, AGS.getMode());
}



unittest_main()

//  -- END OF FILE --
