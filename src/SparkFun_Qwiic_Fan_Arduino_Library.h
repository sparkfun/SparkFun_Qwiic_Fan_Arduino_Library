/*
  This is a library written for the SparkFun Qwiic PC Fan Controller and Qwiic Blower Fan
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/18570
  https://www.sparkfun.com/products/18561

  Written by Nick Poole @ SparkFun Electronics, October 258h, 2021

  The Qwiic PC Fan Controller is an I²C controlled 4-Wire Fan Controller with PI loop control
  The Qwiic Blower Fan is the same controller mounted to a Nintendo Switch Cooling Fan

  https://github.com/sparkfun/SparkFun_Qwiic_Fan_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.7

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SPARKFUN_QWIIC_PC_FAN_LIBRARY_H
#define _SPARKFUN_QWIIC_PC_FAN_LIBRARY_H
#include "Arduino.h"
#include "Wire.h"

#define DEFAULT_WIRE_ADDR 0x36     //7-bit unshifted default I²C Address

#define WIRE_ADDR 0x00             // Device Address
#define FAN_ENABLE 0x01            // Enable power to fan VCC pin (0xFF to enable)
#define SETPOINT_RPM 0x02          // PI Controller setpoint in RPM (2 bytes unsigned integer)
#define KP_VALUE 0x04              // PI Controller Proportional Term (2 bytes signed fixed decimal) \
                                   // Setting this parameter from the I²C Controller overrides the onboard trimpot
#define KI_VALUE 0x06              // PI Controller Integral Term (2 bytes signed fixed decimal) \
                                   // Setting this parameter from the I²C Controller overrides the onboard trimpot
#define FAN_TACH_DIVIDER 0x08      // Pulses per rotation indicated by fan/blower datasheet (usually 2)
#define FAN_ACTUAL_RPM 0x09        // Fan Speed Actual as indicated by Tachometer, input to PI Controller (2 bytes integer)
#define PI_OUT 0x0B                // Output of PI Controller for debugging (2 bytes integer)
#define PI_LOOP_DISABLE 0x0D       // Disable PI Controller and use contents of PROPORTIONAL_THROTTLE as speed (0xFF for Disable)
#define PROPORTIONAL_THROTTLE 0x0E // Fan speed as % of MAX. Inverse of PWM Width Cycle.                          \
                                   // When PI Controller is Enabled this register contains its output x (100/320) \
                                   // Whem PI Controller is Disabled the PWM Width Cycle is set to this register's inverse
#define TRIMMER_SCALE 0x0F         // Trimpot values are multiplied by this number. Probably only useful in edge cases. 2.0 (20) by default.
#define TRIMMER_DISABLE 0x10       // Stop reading trimpot ADCs and only use values written to the Kx_VALUE register by the I²C Controller (0xFF for Disable)

class PCFan {
  public:
    PCFan();

    bool begin(TwoWire &wirePort = Wire, uint8_t deviceAddress = DEFAULT_WIRE_ADDR);
    bool isConnected();                         // Checks if board ACK's the I²C request
	  bool setI2CAddress(uint8_t newAddress);     // Change the I²C address to newAddress
    bool setFanSpeed(uint16_t RPM);             // Set fan speed in RPM (setpoint of PI controller)
    uint16_t getSetpoint();                     // Get current setpoint of PI controller
    uint16_t getFanSpeed();                     // Get current actual fan speed from tachometer
    bool setPIGain(int16_t kp, int16_t ki);     // Set Kp and Ki parameters of PI controller
    float getPGain();                           // Get current Kp of PI controller
    float getIGain();                           // Get current Ki of PI controller
    bool disablePI();                           // Disable PI controller and set fan speed using Percent Throttle instead
    bool enablePI();                            // Enable PI controller 
    bool setTachDivider(uint8_t divider);       // Set tachometer divider in Pulses per Rotation. This number depends on the fan unit but is usually 2
    uint16_t getPIOut();                        // Get last output of PI Controller (20-320)
    bool setPercentThrottle(uint8_t throttle);  // Set fan speed as a percentage of full speed. Disables the PI controller.
    uint8_t getPercentThrottle();               // Get current fan speed as a percentage of full speed (PIOut x (100/320))

    /*THE FOLLOWING FUNCTIONS APPLY ONLY TO QWIIC PC FAN CONTROLLER */

    bool disableFan();                          // Turn off power to the fan
    bool enableFan();                           // Turn on power to the fan
    float getTrimmerScale();                    // Get current scale factor for onboard trimpots
    bool setTrimmerScale(float scale);          // Set scale factor for onboard trimpots
    bool disableTrimmer();                      // Disable the onboard trimpots 
    bool enableTrimmer();                       // Enable the onboard trimpots
	
  private:
    TwoWire *_i2cPort;
	  uint8_t _deviceAddress;
};

#endif
