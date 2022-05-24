/*
  This is a example written for the SparkFun Qwiic PC Fan Controller
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/18570

  Written by Nick Poole @ SparkFun Electronics, October 258h, 2021

  This example code demonstrates how to disable the PI loop and 
  directly control the fan speed as a percentage of the max speed.

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

#include <SparkFun_Qwiic_Fan_Arduino_Library.h>

// Initialize the board
PCFan Fan;

void setup()
{
  Serial.begin(115200);    // Start serial communication at 115200 baud
  
  Serial.println("Qwiic Fan Controller Example 2: Using Percent Throttle to Control Fan Speed");
  if (Fan.begin() == true) // Function to check if the board will ACK
  {
    Serial.println("Begin");
    Fan.enableFan();
  }
  else
  {
    Serial.println("Device failed to setup - stopping");
    while (1); // Just sit and do nothing
  }
}

void loop() {

  Fan.disablePI(); // This is optional, calling setPercentThrottle() will disable the PI controller automatically

  for(byte throttle = 0; throttle <= 100; throttle += 10)
  {
    Fan.setPercentThrottle(throttle); // Set the throttle between 0-100%
    Serial.print("Throttle set to ");
    Serial.print(Fan.getPercentThrottle()); // Trivia: Even with the PI controller enabled, this register will contain the PI output as a percentage
    Serial.println("%");
    Serial.print("Fan Tachometer Reads ");
    Serial.print(Fan.getFanSpeed()); // Let's see what the real speed is at this throttle setting
    Serial.println(" RPM");
    Serial.println();
    delay(3000); // Take a break for 3 seconds
  }

}
