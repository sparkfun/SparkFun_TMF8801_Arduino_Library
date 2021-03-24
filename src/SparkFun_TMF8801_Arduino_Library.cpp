/*
  This is a library written for the AMS TMF-8801 Time-of-flight sensor
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/17716

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 15th, 2021
  This file is the core of the TMF-8801 ToF sensor library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparkFun_TMF8801_Arduino_Library.h"

bool TMF8801::begin(byte address, TwoWire& wirePort)
{
	// Initialize the selected I2C interface 
	bool ready = tmf8801_io.begin(address, wirePort);
	
	// If the interface is not ready or TMF8801 is unreacheable return false
	if (ready == false)
	{
		lastError = ERROR_I2C_COMM_ERROR;
		return false;
	}

	// Reset TMF8801. Since it clears itself, we don't need to clear it
	tmf8801_io.setRegisterBit(REGISTER_ENABLE_REG, CPU_RESET);

	ready = cpuReady();
	if (ready == false)
	{
		lastError = ERROR_CPU_RESET_TIMEOUT;
		return false;
	}

	// Are we really talking to a TMF8801 ?
	byte value = tmf8801_io.readSingleByte(REGISTER_ID);
	if (value != CHIP_ID_NUMBER)
	{
		lastError = ERROR_WRONG_CHIP_ID;
		return false;
	}

	// Load the measurement application and wait until it's ready
	tmf8801_io.writeSingleByte(REGISTER_APPREQID, APPLICATION);
	ready = applicationReady();
	if (ready == false)
	{
		lastError = ERROR_CPU_LOAD_APPLICATION_ERROR;
		return false;
	}

	// Set calibration data
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_CALIBRATION);
	tmf8801_io.writeMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
	tmf8801_io.writeMultipleBytes(REGISTER_STATE_DATA_WR_0, ALGO_STATE, sizeof(ALGO_STATE));

	// Configure the application - values were taken from AN0597, pp. 22
	updateCommandData8();	

	// Start the application
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_MEASURE);

	delay(10);

	// Set lastError no NONE
	lastError = ERROR_NONE;
	return true;
}

bool TMF8801::cpuReady()
{
	short counter = 0;

	// Wait for CPU_READY_TIMEOUT mSec until TMF8801 is ready
	do
	{
		bool ready = tmf8801_io.isBitSet(REGISTER_ENABLE_REG, CPU_READY);
		if (ready == false)
		{
			counter++;
			delay(100);
		}
		else
		{
			return true;
		}
	} while (counter < CPU_READY_TIMEOUT);

	// If TMF8801 CPU is not ready, return false
	return false;
}

bool TMF8801::dataAvailable()
{
	// Returns true if REGISTER_CONTENTS is 0x55
	byte result = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
	return result == COMMAND_RESULT;
}

bool TMF8801::isConnected()
{
	// Polls I2C interface
	bool twiConnected = tmf8801_io.isConnected();
	if (!twiConnected)
		return false;

	// Returns true if TMF8801 ID returned id is 0x07
	return (tmf8801_io.readSingleByte(REGISTER_ID) == CHIP_ID_NUMBER);
}

bool TMF8801::applicationReady()
{
	short counter = 0;

	// Wait for APPLICATION_READY_TIMEOUT mSec until TMF8801 is ready
	do
	{
		bool ready = (tmf8801_io.readSingleByte(REGISTER_APPID) == APPLICATION);
		if (ready == false)
		{
			counter++;
			delay(100);
		}
		else
		{
			return true;
		}
	} while (counter < APPLICATION_READY_TIMEOUT);

	// If application is not ready, return false
	return false;
}

byte TMF8801::getLastError()
{
	return lastError;
}

bool TMF8801::getCalibrationData(byte* calibrationResults)
{
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, 0xff);
	delay(50);

	// Returns device's calibration data values (14 bytes)
	lastError = ERROR_NONE;
	uint32_t calibrationStart = millis();

	byte value;
	do
	{
		tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_FACTORY_CALIBRATION);
		delay(10);
		value = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
		if (value == CONTENT_CALIBRATION)
		{
			delay(10);
			tmf8801_io.readMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationResults, CALIBRATION_DATA_LENGTH);
			return true;
		}
		delay(50);
	} while (millis() - calibrationStart < 30000);
	
	// returns false and writes the lastError if TMF8801 calibration data read operation fails
	lastError = ERROR_FACTORY_CALIBRATION_ERROR;
	return false;
}

void TMF8801::setCalibrationData(const byte* newCalibrationData)
{
	// Copies passed array into calibrationData
	memcpy(calibrationData, newCalibrationData, CALIBRATION_DATA_LENGTH);

	// Reset device with updated values
	resetDevice();
}

byte TMF8801::getApplicationVersionMajor()
{
	return tmf8801_io.readSingleByte(REGISTER_APPREV_MAJOR);
}

byte TMF8801::getApplicationVersionMinor()
{
	return tmf8801_io.readSingleByte(REGISTER_APPREV_MINOR);
}

byte TMF8801::getHardwareVersion()
{
	return tmf8801_io.readSingleByte(REGISTER_REVID);
}

short TMF8801::getSerialNumber()
{
	short serial = 0;
	byte value[2];
	byte result;
	// Request serial number to device
	do
	{	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_SERIAL);
		delay(50);
		result = tmf8801_io.readSingleByte(REGISTER_REGISTER_CONTENTS);
		delay(10);
	} while (result != COMMAND_SERIAL);

	// Read two bytes and combine them as a single int
	tmf8801_io.readMultipleBytes(REGISTER_STATE_DATA_0, value, 2);
	serial = value[1];
	serial = serial << 8;
	serial |= value[0];
	return serial;
}

byte TMF8801::getMeasurementReliability()
{
	// Returns result info without measurement status bits
	return (resultInfo & 0x3f);
}

byte TMF8801::getMeasurementStatus()
{
	// returns resultInfo without measurement reliability bits
	return (resultInfo >> 6);
}

byte TMF8801::getMeasurementNumber()
{
	return resultNumber;
}

void TMF8801::resetDevice()
{
	// Applies newly updated array into main application
	tmf8801_io.setRegisterBit(REGISTER_ENABLE_REG, CPU_RESET);

	// Checks if CPU is ready
	bool ready = false;
	do
	{
		ready = cpuReady();
	} while (!ready);

	// Loads measurement application
	tmf8801_io.writeSingleByte(REGISTER_APPREQID, APPLICATION);
	ready = false;
	do
	{
		ready = applicationReady();
	} while (!ready);

	// Write calibration data and algorithm state into device
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_CALIBRATION);
	tmf8801_io.writeMultipleBytes(REGISTER_FACTORY_CALIB_0, calibrationData, sizeof(calibrationData));
	tmf8801_io.writeMultipleBytes(REGISTER_STATE_DATA_WR_0, ALGO_STATE, sizeof(ALGO_STATE));

	// Updates CMD_DATA_7 to CMD_DATA_0
	updateCommandData8();

	// Start measurements application
	tmf8801_io.writeSingleByte(REGISTER_COMMAND, COMMAND_MEASURE);

	// Wait 50 msec then return
	delay(50);
}

void TMF8801::wakeUpDevice()
{
	byte result;
	// Write ENABLE_REG to bring device back to operation and wait until it's back
	do
	{
		tmf8801_io.writeSingleByte(REGISTER_ENABLE_REG, 0x01);
		result = tmf8801_io.readSingleByte(REGISTER_ENABLE_REG);
		delay(100);
	} while (result != 0x41);
}

byte TMF8801::getStatus()
{
	return tmf8801_io.readSingleByte(REGISTER_STATUS);
}

void TMF8801::doMeasurement()
{
	byte buffer[4];
	tmf8801_io.readMultipleBytes(REGISTER_RESULT_NUMBER, buffer, sizeof(buffer));
	resultNumber = buffer[0];
	resultInfo = buffer[1];
	distancePeak = buffer[3];
	distancePeak = distancePeak << 8;
	distancePeak += buffer[2];
}

int TMF8801::getDistance()
{
	// Returns interrupt pin to open drain
	clearInterruptFlag();
	// Reads measurement data
	doMeasurement();
	return distancePeak;
}

void TMF8801::enableInterrupt()
{
	byte registerValue = tmf8801_io.readSingleByte(REGISTER_INT_ENAB);
	registerValue |= INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(REGISTER_INT_ENAB, registerValue);
	delay(10);
	doMeasurement();
}

void TMF8801::disableInterrupt()
{
	byte registerValue = tmf8801_io.readSingleByte(REGISTER_INT_ENAB);
	registerValue &= ~INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(REGISTER_INT_ENAB, registerValue);
}

void TMF8801::clearInterruptFlag()
{
	byte registerValue = tmf8801_io.readSingleByte(REGISTER_INT_STATUS);
	registerValue |= INTERRUPT_MASK;
	tmf8801_io.writeSingleByte(REGISTER_INT_STATUS, registerValue);
}

void TMF8801::updateCommandData8()
{
	// Writes commandDataValues array into CMD_DATA_7 to CMD_DATA_0 registers
	tmf8801_io.writeMultipleBytes(REGISTER_CMD_DATA7, commandDataValues, sizeof(commandDataValues));
}

bool TMF8801::measurementEnabled()
{
	// Returns true if resultInfo 7:6 are both zeroed
	byte result = resultInfo;
	result = result >> 6;
	return result == 0;
}

void TMF8801::setGPIO0Mode(byte gpioMode)
{
	// Does not allow invalid values to be set into register
	if (gpioMode > MODE_HIGH_OUTPUT)
		return;

	byte currentRegisterValue;

	// Read current value and change only GPIO0 values
	currentRegisterValue = tmf8801_io.readSingleByte(REGISTER_CMD_DATA0);
	currentRegisterValue &= 0xf0;
	currentRegisterValue += gpioMode;
	commandDataValues[CMD_DATA_5] = currentRegisterValue;

	// Send command to device
	byte buffer[2];
	buffer[0] = currentRegisterValue;
	buffer[1] = 0x0f;
	tmf8801_io.writeMultipleBytes(REGISTER_CMD_DATA0, buffer, 2);
}

byte TMF8801::getGPIO0Mode()
{
	// Read REGISTER_CMD_DATA0 and mask accordingly
	byte currentRegisterValue;
	currentRegisterValue = tmf8801_io.readSingleByte(REGISTER_CMD_DATA0);
	return (currentRegisterValue & 0x0f);
}

void TMF8801::setGPIO1Mode(byte gpioMode)
{	
	// Does not allow invalid values to be set into register
	if (gpioMode > MODE_HIGH_OUTPUT)
		return;

	byte currentRegisterValue;

	// Read current value and change only GPIO1 values
	currentRegisterValue = tmf8801_io.readSingleByte(REGISTER_CMD_DATA0);
	currentRegisterValue &= 0x0f;
	currentRegisterValue += (gpioMode << 4);
	commandDataValues[CMD_DATA_5] = currentRegisterValue;

	// Send command to device
	byte buffer[2];
	buffer[0] = currentRegisterValue;
	buffer[1] = 0x0f;
	tmf8801_io.writeMultipleBytes(REGISTER_CMD_DATA0, buffer, 2);
}

byte TMF8801::getGPIO1Mode()
{
	// Read REGISTER_CMD_DATA0 and shift accordingly
	byte currentRegisterValue;
	currentRegisterValue = tmf8801_io.readSingleByte(REGISTER_CMD_DATA0);
	return (currentRegisterValue >> 4);
}

byte TMF8801::getRegisterValue(byte reg)
{
	return tmf8801_io.readSingleByte(reg);
}

void TMF8801::setRegisterValue(byte reg, byte value)
{
	tmf8801_io.writeSingleByte(reg, value);
}

void TMF8801::getRegisterMultipleValues(byte reg, byte* buffer, byte length)
{
	tmf8801_io.readMultipleBytes(reg, buffer, length);
}

void TMF8801::setRegisterMultipleValues(byte reg, const byte* buffer, byte length)
{
	tmf8801_io.writeMultipleBytes(reg, buffer, length);
}
