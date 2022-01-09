/*
  This is a example written for the SparkFun Qwiic Blower Fan
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/18561

  Written by Nick Poole @ SparkFun Electronics, January 8th, 2022

  This example code reports the firmware version of the connected board.

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
PCFan Blower;

void setup()
{
  Serial.begin(115200);    // Start serial communication at 115200 baud
  
  Serial.println("Qwiic Blower Fan Diagnostic: Fetch Firmware Version");
  if (Blower.begin() == true) // Function to check if the board will ACK
  {
    Serial.println("Begin...");
  }
  else
  {
    Serial.println("Device failed to setup - stopping");
    while (1); // Just sit and do nothing
  }

  int fw = Blower.getFirmwareVersion();

  Serial.print("Firmware Version: ");
  Serial.println(fw);
  
}

void loop() {
}
