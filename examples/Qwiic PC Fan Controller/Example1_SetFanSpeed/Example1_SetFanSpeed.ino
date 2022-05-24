/*
  This is a example written for the SparkFun Qwiic PC Fan Controller
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/18570

  Written by Nick Poole @ SparkFun Electronics, October 258h, 2021

  This example code demonstrates how to set and get the fan speed
  by randomly generating fan speeds between 0 and 8000 RPM
  and sending them to the board. It then reads the fan tachometer for 
  comparison.

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
  
  Serial.println("Qwiic Fan Controller Example 1: Setting the Fan Speed");
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

  // Pick a random speed between 0 and 8000 RPM (In 1000 RPM increments)
  int randSpeed = random(0,8) * 1000; 
  
  // Print the number to the terminal
  Serial.print("Setting Fan Speed to ");
  Serial.print(randSpeed); 
  Serial.println(" RPM");
  
  // Set the fan speed
  Fan.setFanSpeed(randSpeed);

  // For the next 3 seconds, once per second, ask the board for a tachometer reading
  // and report it to the terminal
  for(byte i = 0; i < 3; i++){
    delay(1000);
    Serial.print("Tachometer Reads ");
    Serial.print(Fan.getFanSpeed());
    Serial.println(" RPM");
  }

  // Print an empty line to make the terminal more readable
  Serial.println();
  delay(1000);

}
