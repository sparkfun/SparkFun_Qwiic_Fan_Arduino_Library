/*
  This is a example written for the SparkFun Qwiic PC Fan Controller
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/18570

  Written by Nick Poole @ SparkFun Electronics, October 258h, 2021

  This example code demonstrates how to change the proportional and
  integral gain in the PI control loop. In this case, we're going to 
  write the defaults just for demo purposes. The sketch will then
  repeatedly print the current settings, which you should be able 
  to change with the on board trimmers.

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
  
  Serial.println("Qwiic PC Fan Controller Example 4: Changing PI Gain Parameters");
  if (Fan.begin() == true) // Function to check if the board will ACK
  {
    Serial.println("Qwiic PC Fan Controller Connected!");
    Fan.enableFan();
  }
  else
  {
    Serial.println("Qwiic PC Fan Controller failed to connect - stopping");
    while (1); // Just sit and do nothing
  }
}

void loop() {

  Serial.println("Setting Kp to 2.50 and Ki to 3.90");
  Fan.setPIGain(2.50, 3.90); // Kp, Ki

  while(1) 
  {
    Serial.print("Board reports Kp as ");
    Serial.println(Fan.getPGain());
    Serial.print("Board reports Ki as ");
    Serial.println(Fan.getIGain());
    delay(500);
  }

}

/**************************************************
 * Hey, if you're looking at this example then
 * you might also be interested in these functions
 * which don't make much sense to demo, but might
 * be useful in very specific applications:
 *
 *  disablePI(); // Disable PI controller and set fan speed using Percent Throttle instead                       
 *  enablePI(); // Enable the PI controller and use the setpoint for fan speed                         
 *  setTachDivider(); // Set the tachometer divider in Pulses per Rotation. For most fans, this number is 2      
 *  getPIOut(); // Get last output of PI controller (20-320)
 *  getTrimmerScale(); // Get current scale factor for onboard trimpots
 *  setTrimmerScale(float); // Set scale factor for onboard trimpots
 *  disableTrimmer(); // Disable the onboard trimpots 
 *  enableTrimmer(); // Enable the onboard trimpots
 *************************************************/
    
