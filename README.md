# Fall 2025 Senior Design - Section 2, Group 9
## Distribution Plans

### App Subsystem
The application is developed in Android Studio using Kotlin and will be distributed to team members via GitHub. During later stages of development, APK builds will be generated for testing on a physical Android tablet. For final deployment, the app will be packaged as an APK for installation on Android devices. The app interacts with the communications and controls subsystem via it's Bluetooth library, allowing users to send commands and receive data.

### Communication & Control Subsystem
This subsystem has components at the application and embedded levels. The application side covers the custom library which will be used to interact with the glove via Bluetooth LE. This section will follow the distribution plan defined by the app subsystem. For the embedded side, covering the glove's wireless communication and flex sensor data collection and processing, the glove will have a base firmware that it ships with. This can be distributed as a bin file if needed. However, this subsystem also plans to allow users to update the firmware through the app over Bluetooth.

### Actuation Subsystem
The physical glove is controlled by 5 servo motors attached to cables.  Each of them have digitally controlled position 
(utilizing the ESP32's LED pwm channels) and speed. There are 10 solenoids which when in their extended positions prevent a joint 
from bending. The positions of these 10 solenoids are controlled by spi and a 16 bit shift register. In addition to movement
controls, an MAX30102 sensor measures heart rate and blood oxygen level and communicates back to the microcontroller using I2C. This sensor has yet to arrive and be installed.  

