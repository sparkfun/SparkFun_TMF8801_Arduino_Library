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

#include "SparkFun_TMF8801_IO.h"
#include "SparkFun_TMF8801_Constants.h"

bool TMF8801_IO::begin(byte address, TwoWire& wirePort)
{
	_i2cPort = &wirePort;
	_address = address;
	return isConnected();
}

bool TMF8801_IO::isConnected()
{
	_i2cPort->beginTransmission(_address);
	if (_i2cPort->endTransmission() != 0)
		return (false);
	return (true); 
}

void TMF8801_IO::writeMultipleBytes(byte registerAddress, const byte* buffer, byte const packetLength)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	for (byte i = 0; i < packetLength; i++) 
		_i2cPort->write(buffer[i]);
	
	_i2cPort->endTransmission();
}

void TMF8801_IO::readMultipleBytes(byte registerAddress, byte* buffer, byte const packetLength)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->endTransmission();

	_i2cPort->requestFrom(_address, packetLength);
	for (byte i = 0; (i < packetLength) && _i2cPort->available(); i++)
		buffer[i] = _i2cPort->read();
}

byte TMF8801_IO::readSingleByte(byte registerAddress)
{
	byte result;
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, 1U);
	result = _i2cPort->read();
	return result;
}

void TMF8801_IO::writeSingleByte(byte registerAddress, byte const value)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->write(value);
	_i2cPort->endTransmission();
}

void TMF8801_IO::setRegisterBit(byte registerAddress, byte const bitPosition)
{
	byte value = readSingleByte(registerAddress);
	value |= (1 << bitPosition);
	writeSingleByte(registerAddress, value);
}

void TMF8801_IO::clearRegisterBit(byte registerAddress, byte const bitPosition)
{
	byte value = readSingleByte(registerAddress);
	value &= ~(1 << bitPosition);
	writeSingleByte(registerAddress, value);
}

bool TMF8801_IO::isBitSet(byte registerAddress, byte const bitPosition)
{
	byte value = readSingleByte(registerAddress);
	byte mask = 1 << bitPosition;
	if (value & mask)
		return true;
	else
		return false;
}