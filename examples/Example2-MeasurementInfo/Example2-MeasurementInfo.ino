/*
  Using the TMF8801 Time-of-Flight sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: February 22nd, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17716

  This example shows how to read measurement information from the device. 
  The actual measurement status will depend if algImmediateInterrupt is set or not.
  The default value is algImmediateInterrupt cleared (0).
  Further information can be found in the device's datasheet.
  Measurement number is incremented every time there is a unique answer. Value rolls back to zero when it reaches 255 
  and is reset upon device reset.

  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Open a serial monitor at 115200bps
*/

#include <Wire.h>
#include "SparkFun_TMF8801_Arduino_Library.h"

TMF8801 tmf8801;

void setup()
{
  // Start serial @ 115200 bps and wait until it's ready
  Serial.begin(115200);
  while (!Serial) {}

  // Start I2C interface
  Wire.begin();

  // Set the LED_BUILTIN as output
  pinMode(LED_BUILTIN, OUTPUT);

  // Start TMF8801 and print device information...
  if (tmf8801.begin() == true)
  {
    tmf8801.enableInterrupt();
    Serial.print("TMF8801 serial number ");
    Serial.print(tmf8801.getSerialNumber());
    Serial.println(" connected");
    Serial.print("Chip revision: ");
    Serial.println(tmf8801.getHardwareVersion());
    Serial.print("App version: ");
    Serial.print(tmf8801.getApplicationVersionMajor());
    Serial.print(".");
    Serial.println(tmf8801.getApplicationVersionMinor());
  }
  // or an error message if something went wrong.
  else
  {
    Serial.println("TMF8801 connection failed.");
    Serial.print("Status register = 0x");
    Serial.println(tmf8801.getStatus(), HEX);
    printErrorMessage();
    Serial.println("System halted.");
    while (true);
  }
}

void loop()
{
  // Restart the application when ENABLE pin returns to HIGH
  if (!tmf8801.isConnected())
  {
    Serial.println("Not connected or ENABLE pin is low.");
    tmf8801.wakeUpDevice();
    tmf8801.begin();
  }

  // Do we have available data already ?
  if (tmf8801.dataAvailable())
  {
    // Turn on the LED
    digitalWrite(LED_BUILTIN, HIGH);

    // Print out the measurement
    Serial.print("Distance: ");
    Serial.print(tmf8801.getDistance());
    Serial.println(" mm");
    
    // Print out the measurement reliability where 0 = worst, 63 = best
    Serial.print("Measurement reliability (0 = worst, 63 = best): ");
    Serial.println(tmf8801.getMeasurementReliability());
    
    // Print out measurement status. With default values the result will be as follows:
    // 0 Measurement was not interrupted
    // 1 Reserved
    // 2 Measurement was interrupted (delayed) by GPIO interrupt
    // 3 Reserved
    // Further information can be found in TMF8801 datasheet.
    Serial.print("Measurement status: ");
    Serial.println(tmf8801.getMeasurementStatus());

    // Print out the measurement number. This number starts at 0 on reset.
    Serial.print("Measurement number: ");
    Serial.println(tmf8801.getMeasurementNumber());
  }

  // Turn the LED off
  digitalWrite(LED_BUILTIN, LOW);

  // Wait 500 msec and start over
  delay(500);
}

// This function will print a user-friendly error message.
void printErrorMessage()
{
	switch (tmf8801.getLastError())
	{
	case ERROR_I2C_COMM_ERROR:
		Serial.println("Error: I2C communication error");
		break;

	case ERROR_CPU_RESET_TIMEOUT:
		Serial.println("Error: Timeout on CPU reset");
		break;

	case ERROR_WRONG_CHIP_ID:
		Serial.println("Error: Chip ID mismatch");
		break;

	case ERROR_CPU_LOAD_APPLICATION_ERROR:
		Serial.println("Error: Load application error");
		break;

	case ERROR_FACTORY_CALIBRATION_ERROR:
		Serial.println("Error: Calibration was not successful. Please try again.");
		break;

	default:
		break;
	}
}
