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

#ifndef __TMF8801_CONSTANTS__
#define __TMF8801_CONSTANTS__
#include <Arduino.h>
#include <stdint.h>

// Constants definitions
const byte DEFAULT_I2C_ADDR = 0x41;
const byte CPU_READY_TIMEOUT = 200;
const byte APPLICATION_READY_TIMEOUT = 200;
const byte CHIP_ID_NUMBER = 0x07; 
const byte APPLICATION = 0xc0;
const byte BOOTLOADER = 0x80;
const byte COMMAND_CALIBRATION = 0x0b;
const byte COMMAND_FACTORY_CALIBRATION = 0x0a;
const byte COMMAND_MEASURE = 0x02;
const byte COMMAND_RESULT = 0x55;
const byte COMMAND_SERIAL = 0x47;
const byte COMMAND_STOP = 0xff;
const byte INTERRUPT_MASK = 0x01;
const byte CONTENT_CALIBRATION = 0x0a;

// Values below were taken from AN000597, pp 22
const byte ALGO_STATE[11] = { 0xB1, 0xA9, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Error constants
const byte ERROR_NONE = 0x0;
const byte ERROR_I2C_COMM_ERROR = 0x01;
const byte ERROR_CPU_RESET_TIMEOUT = 0x02;
const byte ERROR_WRONG_CHIP_ID = 0x03;
const byte ERROR_CPU_LOAD_APPLICATION_ERROR = 0x04;
const byte ERROR_FACTORY_CALIBRATION_ERROR = 0x05;

// GPIO mode
const byte MODE_INPUT = 0x0;
const byte MODE_LOW_INPUT = 0x01;
const byte MODE_HIGH_INPUT = 0x02;
const byte MODE_VCSEL = 0x03;
const byte MODE_LOW_OUTPUT = 0x04;
const byte MODE_HIGH_OUTPUT = 0x05;

// COMMAND constants
const byte CMD_DATA_7 = 0x0;
const byte CMD_DATA_6 = 0x01;
const byte CMD_DATA_5 = 0x02;
const byte CMD_DATA_4 = 0x03;
const byte CMD_DATA_3 = 0x04;
const byte CMD_DATA_2 = 0x05;
const byte CMD_DATA_1 = 0x06;
const byte CMD_DATA_0 = 0x07;

// CPU status
const byte CPU_RESET= 0X07;
const byte CPU_READY = 0X06;

// Registers definitions
const byte REGISTER_APPID = 0x00;
const byte REGISTER_APPREQID = 0x02;
const byte REGISTER_APPREV_MAJOR = 0x01;
const byte REGISTER_APPREV_MINOR = 0x12;
const byte REGISTER_APPREV_PATCH = 0x13;
const byte REGISTER_CMD_DATA9 = 0x06;
const byte REGISTER_CMD_DATA8 = 0x07;
const byte REGISTER_CMD_DATA7 = 0x08;
const byte REGISTER_CMD_DATA6 = 0x09;
const byte REGISTER_CMD_DATA5 = 0x0A;
const byte REGISTER_CMD_DATA4 = 0x0B;
const byte REGISTER_CMD_DATA3 = 0x0C;
const byte REGISTER_CMD_DATA2 = 0x0D;
const byte REGISTER_CMD_DATA1 = 0x0E;
const byte REGISTER_CMD_DATA0 = 0x0F;
const byte REGISTER_COMMAND = 0x10;
const byte REGISTER_PREVIOUS = 0x11;
const byte REGISTER_STATUS = 0x1D;
const byte REGISTER_REGISTER_CONTENTS = 0x1E;
const byte REGISTER_TID = 0x1F;
const byte REGISTER_RESULT_NUMBER = 0x20;
const byte REGISTER_RESULT_INFO = 0x21;
const byte REGISTER_DISTANCE_PEAK_0 = 0x22;
const byte REGISTER_DISTANCE_PEAK_1 = 0x23;
const byte REGISTER_SYS_CLOCK_0 = 0x24;
const byte REGISTER_SYS_CLOCK_1 = 0x25;
const byte REGISTER_SYS_CLOCK_2 = 0x26;
const byte REGISTER_SYS_CLOCK_3 = 0x27;
const byte REGISTER_STATE_DATA_0 = 0x28;
const byte REGISTER_STATE_DATA_1 = 0x29;
const byte REGISTER_STATE_DATA_2 = 0x2A;
const byte REGISTER_STATE_DATA_3 = 0x2B;
const byte REGISTER_STATE_DATA_4 = 0x2C;
const byte REGISTER_STATE_DATA_5 = 0x2D;
const byte REGISTER_STATE_DATA_6 = 0x2E;
const byte REGISTER_STATE_DATA_7 = 0x2F;
const byte REGISTER_STATE_DATA_8_XTALK_MSB = 0x30;
const byte REGISTER_STATE_DATA_9_XTALK_LSB = 0x31;
const byte REGISTER_STATE_DATA_10_TJ = 0x32;
const byte REGISTER_REFERENCE_HITS_0 = 0x33;
const byte REGISTER_REFERENCE_HITS_1 = 0x34;
const byte REGISTER_REFERENCE_HITS_2 = 0x35;
const byte REGISTER_REFERENCE_HITS_3 = 0x36;
const byte REGISTER_OBJECT_HITS_0 = 0x37;
const byte REGISTER_OBJECT_HITS_1 = 0x38;
const byte REGISTER_OBJECT_HITS_2 = 0x39;
const byte REGISTER_OBJECT_HITS_3 = 0x3A;
const byte REGISTER_FACTORY_CALIB_0 = 0x20;
const byte REGISTER_STATE_DATA_WR_0 = 0x2E;
const byte REGISTER_ENABLE_REG = 0xE0;
const byte REGISTER_INT_STATUS = 0xE1;
const byte REGISTER_INT_ENAB = 0xE2;
const byte REGISTER_ID = 0xE3;
const byte REGISTER_REVID = 0xE4;

// Calibration data
const byte CALIBRATION_DATA_LENGTH = 14;
#endif