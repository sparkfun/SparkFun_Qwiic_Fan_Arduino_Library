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

#include "SparkFun_Qwiic_Fan_Arduino_Library.h"
#include "Arduino.h"

//Constructor
PCFan::PCFan()
{

}

//Initializes the I²C connection
//Returns false if board is not detected
bool PCFan::begin(TwoWire &wirePort, uint8_t deviceAddress)
{
  _i2cPort = &wirePort;
  _i2cPort->begin(); //This resets any setClock() the user may have done

  _deviceAddress = deviceAddress;

  if (isConnected() == false) return (false); //Check for board presence

  return (true); //We're all setup!
}

//Returns true if I²C device ACK's
bool PCFan::isConnected()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress);
  if (_i2cPort->endTransmission() != 0)
    return (false); //Board did not ACK
  return (true);
}

//Change the I²C address of this address to newAddress
bool PCFan::setI2CAddress(uint8_t newAddress)
{
  //Enforce valid I²C address range
  if (8 <= newAddress && newAddress <= 119)
  {
    _i2cPort->beginTransmission((uint8_t)_deviceAddress);
    _i2cPort->write(WIRE_ADDR);
    _i2cPort->write(newAddress);
    if (_i2cPort->endTransmission() != 0)
    {
      return (false); //Device failed to ack
    }   

    //Once the address is changed, we need to change it in the library
    _deviceAddress = newAddress;

    //Restart comms with the new address, throw false if it fails
    return (begin(Wire, newAddress));

  }
  else
  {
    return (false);
  }
  
}

// Set fan speed in RPM (PI controller setpoint) auto-enables the PI controller
bool PCFan::setFanSpeed(uint16_t RPM)
{
  if(piDisabled) //If the PI controller is disabled, we need to enable it
  {
    if(enablePI() == 0) return (false);
    piDisabled = false; //Remember this status
  } 
  _i2cPort->beginTransmission((uint8_t)_deviceAddress);
  _i2cPort->write(SETPOINT_RPM);             
  _i2cPort->write(highByte(RPM));
  _i2cPort->write(lowByte(RPM));  
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  }
  return (true); 
}

// Get the current PI controller setpoint
uint16_t PCFan::getSetpoint()
{
  uint16_t set = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(SETPOINT_RPM);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  }
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 2);
  while(_i2cPort->available()){
    set |= _i2cPort->read() << 8;
    set |= _i2cPort->read();
  }
  return set;
}

// Get the current actual fan speed according to the tachometer
uint16_t PCFan::getFanSpeed()
{
  uint16_t tach = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(FAN_ACTUAL_RPM);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  } 
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 2);
  while(_i2cPort->available()){
    tach |= _i2cPort->read() << 8;
    tach |= _i2cPort->read();
  }
  return tach;
}

// Set Kp and Ki parameters for PI controller
bool PCFan::setPIGain(float kp, float ki)
{
  // We get passed the paramters as floats, for the user's convenience
  // But we treat them as fixed-decimal int16s so we need to do some 
  // quick maths
  kp = constrain(kp, -327.68, 327.67);
  ki = constrain(ki, -327.68, 327.67);
  kp *= 100;
  ki *= 100;
  // static_cast should just truncate the floats
  int16_t kpInt = static_cast<int16_t>(kp);
  int16_t kiInt = static_cast<int16_t>(ki);

  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(KP_VALUE);  
  _i2cPort->write(kpInt >> 8);
  _i2cPort->write(kpInt);                
  _i2cPort->write(kiInt >> 8);
  _i2cPort->write(kiInt);  
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}

// Get the current Kp parameter for PI controller
float PCFan::getPGain()
{
  uint16_t kp = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(KP_VALUE);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  } 
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 2);
  while(_i2cPort->available()){
    kp |= _i2cPort->read() << 8;
    kp |= _i2cPort->read();
  }
  return kp * 0.01;
}

// Get the current Ki parameter for PI controller
float PCFan::getIGain()
{
  uint16_t ki = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(KI_VALUE);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  } 
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 2);
  while(_i2cPort->available()){
    ki |= _i2cPort->read() << 8;
    ki |= _i2cPort->read();
  }
  return ki * 0.01;
}

// Disable the PI controller and use the Percent Throttle value for fan speed 
bool PCFan::disablePI()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(PI_LOOP_DISABLE);
  _i2cPort->write(0xFF);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  piDisabled = true; //Remember this status
  return (true);
}

// Enable the PI controller and use the setpoint for fan speed
bool PCFan::enablePI()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(PI_LOOP_DISABLE); 
  _i2cPort->write(0x00);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  piDisabled = false; //Remember this status
  return (true);
}

// Set the tachometer divider in Pulses per Rotation. For most fans, this number is 2
bool PCFan::setTachDivider(uint8_t divider)
{
  Wire.beginTransmission((uint8_t)_deviceAddress); 
  Wire.write(FAN_TACH_DIVIDER);              
  Wire.write(divider);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}

// Get last output of PI controller (20-320)
uint16_t PCFan::getPIOut()
{
  uint16_t out = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(PI_OUT);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  } 
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 2);
  while(_i2cPort->available()){
    out |= _i2cPort->read() << 8;
    out |= _i2cPort->read();
  }
  return out;
}

// Set the fan speed as a percentage of full speed (Disables the PI controller)
bool PCFan::setPercentThrottle(uint8_t throttle)
{
  if(!piDisabled) //If the PI controller is enabled, it needs to be disabled
  {
    if(disablePI() == 0) return (false);
    piDisabled = true; //Remember this status
  }
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(PROPORTIONAL_THROTTLE);              
  _i2cPort->write(throttle);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}

// Get the current fan speed as a percentage of full speed
uint8_t PCFan::getPercentThrottle()
{
  uint8_t throttle = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(PROPORTIONAL_THROTTLE);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  } 
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 1);
  throttle = _i2cPort->read();
  return throttle;
}

/**********************************************************
 * The Following Functions address features which exist
 * ONLY on the Qwiic PC Fan Controller board. Calling 
 * these functions while using the Qwiic Blower Fan may
 * result in unexpected or erratic behavior.
**********************************************************/

// Turn off power to the fan 
bool PCFan::disableFan()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(FAN_ENABLE);
  _i2cPort->write(0xFF);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}

// Turn on power to the fan
bool PCFan::enableFan()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(FAN_ENABLE); 
  _i2cPort->write(0x00);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}

// Get the current scale factor for the onboard trimpots (-12.8 to 12.7)
float PCFan::getTrimmerScale()
{
  int8_t scale = 0;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(TRIMMER_SCALE);              
  if (_i2cPort->endTransmission() != 0)
  {
    return (0); //Device failed to ack
  } 
  _i2cPort->requestFrom((uint8_t)_deviceAddress, 1);
  scale = static_cast<int8_t>(_i2cPort->read()); //Paranoid explicit casting
  return scale * 0.1;
}

// Get the scale factor for the onboard trimpots (-12.8 to 12.7)
bool PCFan::setTrimmerScale(float scale)
{
  scale = constrain(scale, -12.8, 12.7);
  scale *= 10;
  _i2cPort->beginTransmission((uint8_t)_deviceAddress);
  _i2cPort->write(TRIMMER_SCALE);             
  _i2cPort->write(static_cast<uint8_t>(scale)); //Paranoid explicit casting
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  }
  return (true); 
}

// Disable onboard trimpots
bool PCFan::disableTrimmer()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(TRIMMER_DISABLE);
  _i2cPort->write(0xFF);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}

// Enable onboard trimpots
bool PCFan::enableTrimmer()
{
  _i2cPort->beginTransmission((uint8_t)_deviceAddress); 
  _i2cPort->write(TRIMMER_DISABLE); 
  _i2cPort->write(0x00);
  if (_i2cPort->endTransmission() != 0)
  {
    return (false); //Device failed to ack
  } 
  return (true);
}