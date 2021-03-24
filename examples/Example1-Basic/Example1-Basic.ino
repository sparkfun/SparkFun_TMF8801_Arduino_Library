/*
  Using the TMF8801 Time-of-Flight sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: February 22nd, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17716

  This example shows how to read basic distance from a TMF8801 device.

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
	// or an error message i something went wrong.
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
