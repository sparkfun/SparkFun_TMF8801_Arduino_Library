/*
  Using the TMF8801 Time-of-Flight sensor
  By: Ricardo Ramos
  SparkFun Electronics
  Date: February 27th, 2021
  SparkFun code, firmware, and software is released under the MIT License. Please see LICENSE.md for further details.
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/17716

  This example shows how to have TMF8801 interrupt the microcontroller using it's INT pin.

  Hardware Connections:
  - Plug the Qwiic device to your Arduino/Photon/ESP32 using a cable
  - Connect a jumper wire between Arduino pin 2 and INT pin header in TMF8801 board
  - Open a serial monitor at 115200bps
*/

#include <Wire.h>
#include "SparkFun_TMF8801_Arduino_Library.h"

TMF8801 tmf8801;
const byte interruptPin = 2;
volatile bool getData;

void setup()
{
  // Start serial @ 115200 bps and wait until it's ready
  Serial.begin(115200);
  while (!Serial) {}

  // Start I2C interface
  Wire.begin();

  // Set the LED_BUILTIN as output
  pinMode(LED_BUILTIN, OUTPUT);
  // Set chosen interrupt pin as input
  pinMode(interruptPin, INPUT);
  // Enable interrupt service routine to be triggered by the rising edge of interruptPin
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptServiceRoutine, FALLING);

  getData = false;
  
  // Start TMF8801 and print device information...
  if(tmf8801.begin() == true)
  {
    Serial.print("TMF8801 serial number ");
    Serial.print(tmf8801.getSerialNumber());
    Serial.println(" connected");
    Serial.print("Chip revision: ");
    Serial.println(tmf8801.getHardwareVersion());
    Serial.print("App version: ");
    Serial.print(tmf8801.getApplicationVersionMajor());
    Serial.print(".");
    Serial.println(tmf8801.getApplicationVersionMinor());
    
    // Enable interrupt generation
    tmf8801.enableInterrupt();
  }
  // or an error message if something went wrong.
  else
  {
    Serial.println("TMF8801 connection failed.");
    Serial.print("Status register = 0x");
    Serial.println(tmf8801.getStatus(), HEX);
    printErrorMessage();
    Serial.println("System halted.");
    while (true) ;
  }
}

void loop()
{
  // Restart the application when ENABLE pin returns to HIGH
  if(!tmf8801.isConnected())
  {
    Serial.println("Not connected or ENABLE pin is low.");
    tmf8801.wakeUpDevice();
    tmf8801.begin();
  }
  
  // getData gets set by the interrupt service routine, if it's true
  // the device has updated data
  if(getData == true)
  {
    // Do we have available data already ?
    if(tmf8801.dataAvailable() == true)
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
    // clear getData flag so we can reuse it
    getData = false;
  }
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

// This function will be called whenever TMF8801 has available data to be read
void interruptServiceRoutine()
{
  // Just set the flag to indicate that there's data available
  getData = true;
}
