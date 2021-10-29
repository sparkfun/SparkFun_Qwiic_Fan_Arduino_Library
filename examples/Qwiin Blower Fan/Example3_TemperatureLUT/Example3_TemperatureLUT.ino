/*
  This is a example written for the SparkFun Qwiic Blower Fan
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/18561

  Written by Nick Poole @ SparkFun Electronics, October 258h, 2021

  This example code demonstrates how to control the fan speed output
  in response to a connected TMP102 temperature sensor and a static
  Lookup Table. The lookup table is not elegantly implemented but 
  it should give you an idea of how to accomplish thermal control.

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
#include <SparkFunTMP102.h> //http://librarymanager/All#SparkFun_TMP102

// Initialize the board
PCFan Blower;
TMP102 TempSensor;

// LOOKUP TABLE
// {TempC, FanSpeed%}
const int LUT[][2] = {
  {20, 0},
  {26, 60},
  {35, 90}
};

const int sizeofLUT = (sizeof(LUT) / sizeof(LUT[0]));

void setup()
{
  Serial.begin(115200);    // Start serial communication at 115200 baud
  
  Serial.println("Qwiic Blower Fan Example 3: Thermal Fan Speed Control with TMP102 and LUT");
  if (Blower.begin() == true) // Function to check if the board will ACK
  {
    Serial.println("Qwiic Blower Fan Connected!");
  }
  else
  {
    Serial.println("Qwiic Blower Fan failed to connect - stopping");
    while (1); // Just sit and do nothing
  }
  if(TempSensor.begin() == true)
  {
    Serial.println("TMP102 Connected!");  
    delay(100);
    TempSensor.sleep(); // Put sensor to sleep
  }
  else
  {
    Serial.println("TMP102 failed to connect - stopping");
    while (1); // Just sit and do nothing
  }
}

void loop() {

  // Get temperature reading from TMP102
  TempSensor.oneShot(1); // Set One-Shot bit on the TMP102
  while(TempSensor.oneShot() == 0); // Wait for conversion to be ready
  float tempC = TempSensor.readTempC();
  TempSensor.sleep();

  // Find the first entry in the LUT that's bigger than the temp reading
  byte idx = 0;
  while(LUT[idx][0] <= tempC)
  {
    idx++;
    if (idx == sizeofLUT) break; // If our reading is bigger than any table entry, stop looking and over-index
  }

  // In this demo, we assume that the first table entry is the min output point
  // and the last table entry is the max output point
  // So if our reading is below the threshold of the first table entry, we 
  // just set the output to 0 and if it's above the threshold of our last
  // table entry, we set the output to that.
  if(idx == 0) // min
  {
    Blower.setPercentThrottle(LUT[0][1]);
  }
  else if(idx == sizeofLUT) // max
  {
    Blower.setPercentThrottle(LUT[idx-1][1]);
  }
  else // linear interpolate
  {
    float scale = map(tempC, LUT[idx-1][0], LUT[idx][0], 0, 100); // Find out "how far" between the two nearest table entries our temperature reading is
    float output = map(scale, 0, 100, LUT[idx-1][1], LUT[idx][1]); // Now use that scale factor to interpolate between the fan output entries in our table
    Blower.setPercentThrottle(output); // Output is a float but we'll just let it get truncated for this demo
  }

  delay(5000); // No need to respond TOO quickly

}
