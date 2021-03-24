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

#ifndef __TMF8801_LIBRARY_IO__
#define __TMF8801_LIBRARY_IO__

#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_TMF8801_Constants.h"

class TMF8801_IO
{
private:
	TwoWire* _i2cPort;
	byte _address;

public:
	// Default constructor.
	TMF8801_IO() {}

	// Starts two wire interface.
	bool begin(byte address, TwoWire& wirePort);

	// Returns true if we get a reply from the I2C device.
	bool isConnected();

	// Read a single byte from a register.
	byte readSingleByte(byte registerAddress);

	// Writes a single byte into a register.
	void writeSingleByte(byte registerAddress, byte value);

	// Reads multiple bytes from a register into buffer byte array.
	void readMultipleBytes(byte registerAddress, byte* buffer, byte packetLength);

	// Writes multiple bytes to register from buffer byte array.
	void writeMultipleBytes(byte registerAddress, const byte* buffer, byte packetLength);

	// Sets a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
	void setRegisterBit(byte registerAddress, byte bitPosition);

	// Clears a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
	void clearRegisterBit(byte registerAddress, byte bitPosition);

	// Returns true if a specific bit is set in a register. Bit position ranges from 0 (lsb) to 7 (msb).
	bool isBitSet(byte registerAddress, byte bitPosition);
};
#endif