
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
  digitalWrite(LED_BUILTIN, LOW);
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

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  // Start TMF8801 and print device information...
  if (tmf8801.begin() == true)
  {
    Serial.print("TMF8801 serial number ");
    Serial.print(tmf8801.getSerialNumber());
    Serial.println(" connected");
    Serial.print("Chip revision: ");
    Serial.print(tmf8801.getHardwareVersion());
    Serial.print(" - App version: ");
    Serial.print(tmf8801.getApplicationVersionMajor());
    Serial.print(".");
    Serial.println(tmf8801.getApplicationVersionMinor());
    digitalWrite(LED_BUILTIN, HIGH);
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
  delay(500);
}
