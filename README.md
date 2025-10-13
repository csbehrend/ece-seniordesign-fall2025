# Distribution Plans

## App Subsystem
The application is developed in Android Studio using Kotlin and will be distributed to team members via GitHub.  
During later stages of development, APK builds will be generated for testing on a physical Android tablet. 
For final deployment, the app will be packaged as an APK for installation on Android devices.
The app interacts with the communications and controls subsystem via Bluetooth, allowing users to send commands and receive data.

## Actuation Subsystem
The physical glove is controlled by 5 servo motors attached to cables.  Each of them have digitally controlled position 
(utilizing the ESP32's LED pwm channels) and speed.  There are 10 solenoids which when in their extended positions prevent a joint 
from bending.  The positions of these 10 solenoids are controlled by spi and a 16 bit shift register.  In addition to movement
controls, an MAX30102 sensor measures heart rate and blood oxygen level and communicates back to the microcontroller using I2C.   This sensor has yet to arrive and be installed.  

